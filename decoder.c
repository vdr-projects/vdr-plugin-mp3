/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2009 Stefan Huelswitt <s.huelswitt@gmx.de>
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/vfs.h>

#include <vdr/videodir.h>

#include "common.h"
#include "data-mp3.h"
#include "data-src.h"
#include "decoder.h"
#include "decoder-core.h"
#include "decoder-mp3.h"
#include "decoder-mp3-stream.h"
#include "decoder-snd.h"
#include "decoder-ogg.h"
#include "decoder-ogg-stream.h"

#define CACHEFILENAME     "id3info.cache"
#define CACHESAVETIMEOUT  120 // secs
#define CACHEPURGETIMEOUT 120 // days

extern cFileSources MP3Sources;

cInfoCache InfoCache;
char *cachedir=0;

int MakeHashBuff(const char *buff, int len)
{
  int h=len;
  while(len--) h=(h*13 + *buff++) & 0x7ff;
  return h;
}

// --- cStrConv ----------------------------------------------------------------

class cStrConv : private cMutex {
private:
  cCharSetConv toSys;
public:
  cStrConv(void):toSys("UTF-8",cCharSetConv::SystemCharacterTable()) {}
  char *ToSys(char *from);
  };

static cStrConv *strconv;
  
char *cStrConv::ToSys(char *from)
{
  if(from) {
    Lock();
    const char *r=toSys.Convert(from);
    Unlock();
    if(r!=from) {
      char *n=strdup(r);
      if(n) {
        free(from);
        return n;
        }
      }
    }
  return from;
}

// --- cSongInfo ---------------------------------------------------------------

cSongInfo::cSongInfo(void)
{
  Title=Artist=Album=0;
  Clear();
}

cSongInfo::~cSongInfo()
{
  Clear();
}

void cSongInfo::Clear(void)
{
  Frames=0; Total=-1; DecoderID=0;
  SampleFreq=Channels=Bitrate=MaxBitrate=ChMode=-1;
  free(Title); Title=0;
  free(Artist); Artist=0;
  free(Album); Album=0;
  Year=-1;
  Level=Peak=0.0;
  infoDone=false; utf8clean=true;
}

void cSongInfo::Set(cSongInfo *si, bool update)
{
  if(!update || si->Utf8Clean()) {
    Clear();
    Title=si->Title ? strdup(si->Title):0;
    Artist=si->Artist ? strdup(si->Artist):0;
    Album=si->Album ? strdup(si->Album):0;
    utf8clean=si->utf8clean;
    }
  Frames=si->Frames;
  Total=si->Total;
  SampleFreq=si->SampleFreq;
  Channels=si->Channels;
  Bitrate=si->Bitrate;
  MaxBitrate=si->MaxBitrate;
  ChMode=si->ChMode;
  Year=si->Year;
  if(si->Level>0.0) { // preserve old level
    Level=si->Level;
    Peak=si->Peak;
    }
  DecoderID=si->DecoderID;
  InfoDone();
}

void cSongInfo::FakeTitle(const char *filename, const char *extention)
{
  // if no title, try to build a reasonable from the filename
  if(!Title && filename)  {
    const char *s=rindex(filename,'/');
    if(s && *s=='/') {
      s++;
      Title=strdup(s);
      strreplace(Title,'_',' ');
      if(extention) {                            // strip given extention
        int l=strlen(Title)-strlen(extention);
        if(l>0 && !strcasecmp(Title+l,extention)) Title[l]=0;
        }
      else {                                     // strip any extention
        char *e=rindex(Title,'.');
        if(e && *e=='.' && strlen(e)<=5) *e=0;
        }
      d(printf("mp3: faking title '%s' from filename '%s'\n",Title,filename))
      }
    }
}

void cSongInfo::ConvertToSys(void)
{
  if(cCharSetConv::SystemCharacterTable()) {
    Title=strconv->ToSys(Title);
    Artist=strconv->ToSys(Artist);
    Album=strconv->ToSys(Album);
    utf8clean=false;
    }
}

// --- cFileInfo ---------------------------------------------------------------

cFileInfo::cFileInfo(void)
{
  Filename=FsID=0; Clear();
}

cFileInfo::cFileInfo(const char *Name)
{
  Filename=FsID=0; Clear();
  Filename=strdup(Name);
}

cFileInfo::~cFileInfo()
{
  Clear();
}

void cFileInfo::Clear(void)
{
  free(Filename); Filename=0;
  free(FsID); FsID=0;
  Filesize=0; CTime=0; FsType=0; removable=-1;
  infoDone=false;
}

bool cFileInfo::Removable(void)
{
  if(removable<0 && Filename) {
    cFileSource *src=MP3Sources.FindSource(Filename);
    if(src) removable=src->NeedsMount();
    else removable=1;
    }
  return (removable!=0);
}

void cFileInfo::Set(cFileInfo *fi)
{
  Clear(); InfoDone();
  Filename=fi->Filename ? strdup(fi->Filename):0;
  FsID=fi->FsID ? strdup(fi->FsID):0;
  Filesize=fi->Filesize;
  CTime=fi->CTime;
}


bool cFileInfo::FileInfo(bool log)
{
  if(Filename) {
    struct stat64 ds;
    if(!stat64(Filename,&ds)) {
      if(S_ISREG(ds.st_mode)) {
        free(FsID); FsID=0;
        FsType=0;
        struct statfs64 sfs;
        if(!statfs64(Filename,&sfs)) {
          if(Removable()) FsID=aprintf("%llx:%llx",sfs.f_blocks,sfs.f_files);
          FsType=sfs.f_type;
          }
        else if(errno!=ENOSYS && log) { esyslog("ERROR: can't statfs %s: %s",Filename,strerror(errno)); }
        Filesize=ds.st_size;
        CTime=ds.st_ctime;
#ifdef CDFS_MAGIC
        if(FsType==CDFS_MAGIC) CTime=0; // CDFS returns mount time as ctime
#endif
        InfoDone();
        return true;
        }
      else if(log) { esyslog("ERROR: %s is not a regular file",Filename); }
      }
    else if(log) { esyslog("ERROR: can't stat %s: %s",Filename,strerror(errno)); }
    }
  return false;
}

// --- cDecoders ---------------------------------------------------------------

cDecoder *cDecoders::FindDecoder(cFileObj *Obj)
{
  const char *full=Obj->FullPath();
  cFileInfo fi(full);
  cCacheData *dat;
  cDecoder *decoder=0;
  if(fi.FileInfo(false) && (dat=InfoCache.Search(&fi))) {
    if(dat->DecoderID) {
      //d(printf("mp3: found DecoderID '%s' for %s from cache\n",cDecoders::ID2Str(dat->DecoderID),Filename))
      switch(dat->DecoderID) {
        case DEC_MP3:  decoder=new cMP3Decoder(full); break;
        case DEC_MP3S: decoder=new cMP3StreamDecoder(full); break;
#ifdef HAVE_SNDFILE
        case DEC_SND:  decoder=new cSndDecoder(full); break;
#endif
#ifdef HAVE_VORBISFILE
        case DEC_OGG:  decoder=new cOggDecoder(full); break;
        case DEC_OGGS: decoder=new cOggStreamDecoder(full); break;
#endif
        default:       esyslog("ERROR: bad DecoderID '%s' from info cache: %s",cDecoders::ID2Str(dat->DecoderID),full); break;
        }
      }
    dat->Unlock();
    }

  if(!decoder || !decoder->Valid()) {
    // no decoder in cache or cached decoder doesn't matches.
    // try to detect a decoder

    delete decoder; decoder=0;
#ifdef HAVE_SNDFILE
    if(!decoder) {
      decoder=new cSndDecoder(full);
      if(!decoder || !decoder->Valid()) { delete decoder; decoder=0; }
      }
#endif
#ifdef HAVE_VORBISFILE
    if(!decoder) {
      decoder=new cOggDecoder(full);
      if(!decoder || !decoder->Valid()) { delete decoder; decoder=0; }
      }
    if(!decoder) {
      decoder=new cOggStreamDecoder(full);
      if(!decoder || !decoder->Valid()) { delete decoder; decoder=0; }
      }
#endif
    if(!decoder) {
      decoder=new cMP3StreamDecoder(full);
      if(!decoder || !decoder->Valid()) { delete decoder; decoder=0; }
      }
    if(!decoder) {
      decoder=new cMP3Decoder(full);
      if(!decoder || !decoder->Valid()) { delete decoder; decoder=0; }
      }
    if(!decoder) esyslog("ERROR: no decoder found for %s",Obj->Name());
    }
  return decoder;
}

const char *cDecoders::ID2Str(int id)
{
  switch(id) {
    case DEC_MP3:  return DEC_MP3_STR;
    case DEC_MP3S: return DEC_MP3S_STR;
    case DEC_SND:  return DEC_SND_STR;
    case DEC_OGG:  return DEC_OGG_STR;
    case DEC_OGGS: return DEC_OGGS_STR;
    }
  return 0;
}

int cDecoders::Str2ID(const char *str)
{
  if     (!strcmp(str,DEC_MP3_STR )) return DEC_MP3;
  else if(!strcmp(str,DEC_MP3S_STR)) return DEC_MP3S;
  else if(!strcmp(str,DEC_SND_STR )) return DEC_SND;
  else if(!strcmp(str,DEC_OGG_STR )) return DEC_OGG;
  else if(!strcmp(str,DEC_OGGS_STR)) return DEC_OGGS;
  return 0;
}

// --- cDecoder ----------------------------------------------------------------

cDecoder::cDecoder(const char *Filename)
{
  filename=strdup(Filename);
  locked=0; urgentLock=playing=false;
}

cDecoder::~cDecoder()
{
  free(filename);
}

void cDecoder::Lock(bool urgent)
{
  locklock.Lock();
  if(urgent && locked) urgentLock=true; // signal other locks to release quickly
  locked++;
  locklock.Unlock(); // don't hold the "locklock" when locking "lock", may cause a deadlock
  lock.Lock();
  urgentLock=false;
}

void cDecoder::Unlock(void)
{
  locklock.Lock();
  locked--;
  lock.Unlock();
  locklock.Unlock();
}

bool cDecoder::TryLock(void)
{
  bool res=false;
  locklock.Lock();
  if(!locked && !playing) {
    Lock();
    res=true;
    }
  locklock.Unlock();
  return res;
}

// --- cCacheData -----------------------------------------------------

cCacheData::cCacheData(void)
{
  touch=0; version=0;
}

void cCacheData::Touch(void)
{
  touch=time(0);
}

#define SECS_PER_DAY (24*60*60)

bool cCacheData::Purge(void)
{
  time_t now=time(0);
  //XXX does this realy made sense?
  //if(touch+CACHEPURGETIMEOUT*SECS_PER_DAY < now) {
  //  d(printf("cache: purged: timeout %s\n",Filename))
  //  return true;
  //  }
  if(touch+CACHEPURGETIMEOUT*SECS_PER_DAY/10 < now) {
    if(!Removable()) {                            // is this a permant source?
      struct stat64 ds;                           // does the file exists? if not, purge
      if(stat64(Filename,&ds) || !S_ISREG(ds.st_mode) || access(Filename,R_OK)) {
        d(printf("cache: purged: file not found %s\n",Filename))
        return true;
        }
      }
    }
  return false;
}

bool cCacheData::Check8bit(const char *str)
{
  if(str) while(*str) if(*str++ & 0x80) return true;
  return false;
}

bool cCacheData::Upgrade(void)
{
  if(version<8) {
    if(Check8bit(Title) || Check8bit(Artist) || Check8bit(Album))
      return false;              // Trash entries not 7bit clean
    }
  if(version<7) {
    if(DecoderID==DEC_SND || (Title && startswith(Title,"track-")))
      return false;              // Trash older SND entries (incomplete)

    if(Removable()) {
      if(!FsID) FsID=strdup("old"); // Dummy entry, will be replaced in InfoCache::Search()
      }
    else { free(FsID); FsID=0; }
    }
  if(version<4) {
    Touch();                     // Touch entry
    }
  if(version<3 && !Title) {
    FakeTitle(Filename,".mp3");  // Fake title
    }
  if(version<2 && Bitrate<=0) {
    return false;                // Trash entry without bitrate
    }
  return true;
}

void cCacheData::Create(cFileInfo *fi, cSongInfo *si, bool update)
{
  cFileInfo::Set(fi);
  cSongInfo::Set(si,update);
  hash=MakeHash(Filename);
  Touch();
}

bool cCacheData::Save(FILE *f)
{
  fprintf(f,"##BEGIN\n"
            "Filename=%s\n"
            "Filesize=%lld\n"
            "Timestamp=%ld\n"
            "Touch=%ld\n"
            "Version=%d\n"
            "Frames=%d\n"
            "Total=%d\n"
            "SampleFreq=%d\n"
            "Channels=%d\n"
            "Bitrate=%d\n"
            "MaxBitrate=%d\n"
            "ChMode=%d\n"
            "Year=%d\n"
            "Level=%.4f\n"
            "Peak=%.4f\n",
            Filename,Filesize,CTime,touch,CACHE_VERSION,Frames,Total,SampleFreq,Channels,Bitrate,MaxBitrate,ChMode,Year,Level,Peak);
  if(Title)     fprintf(f,"Title=%s\n"    ,Title);
  if(Artist)    fprintf(f,"Artist=%s\n"   ,Artist);
  if(Album)     fprintf(f,"Album=%s\n"    ,Album);
  if(DecoderID) fprintf(f,"DecoderID=%s\n",cDecoders::ID2Str(DecoderID));
  if(FsID)      fprintf(f,"FsID=%s\n"     ,FsID);
  fprintf(f,"##END\n");
  return ferror(f)==0;
}

bool cCacheData::Load(FILE *f)
{
  static const char delimiters[] = { "=\n" };
  char buf[1024];

  cFileInfo::Clear();
  cSongInfo::Clear();
  while(fgets(buf,sizeof(buf),f)) {
    char *ptrptr;
    char *name =strtok_r(buf ,delimiters,&ptrptr);
    char *value=strtok_r(0,delimiters,&ptrptr);
    if(name) {
      if(!strcasecmp(name,"##END")) break;
      if(value) {
        if     (!strcasecmp(name,"Filename"))   Filename  =strdup(value);
        else if(!strcasecmp(name,"Filesize") ||
                !strcasecmp(name,"Size"))       Filesize  =atoll(value);
        else if(!strcasecmp(name,"FsID"))       FsID      =strdup(value);
        else if(!strcasecmp(name,"Timestamp"))  CTime     =atol(value);
        else if(!strcasecmp(name,"Touch"))      touch     =atol(value);
        else if(!strcasecmp(name,"Version"))    version   =atoi(value);
        else if(!strcasecmp(name,"DecoderID"))  DecoderID =cDecoders::Str2ID(value);
        else if(!strcasecmp(name,"Frames"))     Frames    =atoi(value);
        else if(!strcasecmp(name,"Total"))      Total     =atoi(value);
        else if(!strcasecmp(name,"SampleFreq")) SampleFreq=atoi(value);
        else if(!strcasecmp(name,"Channels"))   Channels  =atoi(value);
        else if(!strcasecmp(name,"Bitrate"))    Bitrate   =atoi(value);
        else if(!strcasecmp(name,"MaxBitrate")) MaxBitrate=atoi(value);
        else if(!strcasecmp(name,"ChMode"))     ChMode    =atoi(value);
        else if(!strcasecmp(name,"Year"))       Year      =atoi(value);
        else if(!strcasecmp(name,"Title"))      Title     =strdup(value);
        else if(!strcasecmp(name,"Artist"))     Artist    =strdup(value);
        else if(!strcasecmp(name,"Album"))      Album     =strdup(value);
        else if(!strcasecmp(name,"Level"))      Level     =atof(value);
        else if(!strcasecmp(name,"Peak"))       Peak      =atof(value);
        else d(printf("cache: ignoring bad token '%s' from cache file\n",name))
        }
      }
    }

  if(ferror(f) || !Filename) return false;
  hash=MakeHash(Filename);
  return true;
}

// --- cInfoCache ----------------------------------------------------

cInfoCache::cInfoCache(void)
{
  lasttime=0; modified=false;
  lastpurge=time(0)-(50*60);
}

void cInfoCache::Shutdown(void)
{
  Cancel(10);
  Save(true);
}

void cInfoCache::Cache(cSongInfo *info, cFileInfo *file)
{
  lock.Lock();
  cCacheData *dat=Search(file);
  if(dat) {
    dat->Create(file,info,true);
    Modified();
    dat->Unlock();
    d(printf("cache: updating infos for %s\n",file->Filename))
    }
  else {
    dat=new cCacheData;
    dat->Create(file,info,false);
    AddEntry(dat);
    d(printf("cache: caching infos for %s\n",file->Filename))
    }
  lock.Unlock();
}

cCacheData *cInfoCache::Search(cFileInfo *file)
{
  int hash=MakeHash(file->Filename);
  lock.Lock();
  cCacheData *dat=FirstEntry(hash);
  while(dat) {
    if(dat->hash==hash && !strcmp(dat->Filename,file->Filename) && dat->Filesize==file->Filesize) {
      dat->Lock();
      if(file->FsID && dat->FsID && !strcmp(dat->FsID,"old")) { // duplicate FsID for old entries
        dat->FsID=strdup(file->FsID);
        dat->Touch(); Modified();
        //d(printf("adding FsID for %s\n",dat->Filename))
        }

      if((!file->FsID && !dat->FsID) || (file->FsID && dat->FsID && !strcmp(dat->FsID,file->FsID))) {
        //d(printf("cache: found cache entry for %s\n",dat->Filename))
        dat->Touch(); Modified();
        if(dat->CTime!=file->CTime) {
          d(printf("cache: ctime differs, removing from cache: %s\n",dat->Filename))
          DelEntry(dat); dat=0;
          }
        break;
        }
      dat->Unlock();
      }
    dat=(cCacheData *)dat->Next();
    }
  lock.Unlock();
  return dat;
}

void cInfoCache::AddEntry(cCacheData *dat)
{
  lists[dat->hash%CACHELINES].Add(dat);
  Modified();
}

void cInfoCache::DelEntry(cCacheData *dat)
{
  dat->Lock();
  lists[dat->hash%CACHELINES].Del(dat);
  Modified();
}

cCacheData *cInfoCache::FirstEntry(int hash)
{
  return lists[hash%CACHELINES].First();
}

bool cInfoCache::Purge(void)
{
  time_t now=time(0);
  if(now-lastpurge>(60*60)) {
    isyslog("cleaning up id3 cache");
    Start();
    lastpurge=now;
    }
  return Active();
}

void cInfoCache::Action(void)
{
  d(printf("cache: id3 cache purge thread started (pid=%d)\n",getpid()))
  if(nice(3)<0);
  lock.Lock();
  for(int i=0,n=0 ; i<CACHELINES && Running(); i++) {
    cCacheData *dat=FirstEntry(i);
    while(dat && Running()) {
      cCacheData *ndat=(cCacheData *)dat->Next();
      if(dat->Purge()) DelEntry(dat);
      dat=ndat;

      if(++n>30) {
        lastmod=false; n=0;
        lock.Unlock(); lock.Lock();    // give concurrent thread an access chance
        if(lastmod) dat=FirstEntry(i); // restart line, if cache changed meanwhile
        }
      }
    }
  lock.Unlock();
  d(printf("cache: id3 cache purge thread ended (pid=%d)\n",getpid()))
}

char *cInfoCache::CacheFile(void)
{
#if APIVERSNUM > 20101
  return AddPath(cachedir?cachedir:cVideoDirectory::Name(),CACHEFILENAME);
#else
  return AddPath(cachedir?cachedir:VideoDirectory,CACHEFILENAME);
#endif
}

void cInfoCache::Save(bool force)
{
  if(modified && (force || (!Purge() && time(0)>lasttime))) {
    char *name=CacheFile();
    cSafeFile f(name);
    free(name);
    if(f.Open()) {
      lock.Lock();
      fprintf(f,"## This is a generated file. DO NOT EDIT!!\n"
                "## This file will be OVERWRITTEN WITHOUT WARNING!!\n");
      for(int i=0 ; i<CACHELINES ; i++) {
        cCacheData *dat=FirstEntry(i);
        while(dat) {
          if(!dat->Save(f)) { i=CACHELINES+1; break; }
          dat=(cCacheData *)dat->Next();
          }
        }
      lock.Unlock();
      f.Close();
      modified=false; lasttime=time(0)+CACHESAVETIMEOUT;
      d(printf("cache: saved cache to file\n"))
      }
    }
}

void cInfoCache::Load(void)
{
  if(!strconv) strconv=new cStrConv;

  char *name=CacheFile();
  if(access(name,F_OK)==0) {
    isyslog("loading id3 cache from %s",name);
    FILE *f=fopen(name,"r");
    if(f) {
      char buf[256];
      bool mod=false;
      lock.Lock();
      for(int i=0 ; i<CACHELINES ; i++) lists[i].Clear();
      while(fgets(buf,sizeof(buf),f)) {
        if(!strcasecmp(buf,"##BEGIN\n")) {
          cCacheData *dat=new cCacheData;
          if(dat->Load(f)) {
            if(dat->version!=CACHE_VERSION) {
              if(dat->Upgrade()) mod=true;
              else { delete dat; continue; }
              }
            AddEntry(dat);
            }
          else {
            delete dat;
            if(ferror(f)) {
              esyslog("ERROR: failed to load id3 cache");
              break;
              }
            }
          }
        }
      lock.Unlock();
      fclose(f);
      modified=false; if(mod) Modified();
      }
    else LOG_ERROR_STR(name);
    }
  free(name);
}
