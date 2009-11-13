/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2009 Stefan Huelswitt <s.huelswitt@gmx.de>
 *
 * OGG stream support initialy developed by Manuel Reimer <manuel.reimer@gmx.de>
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

#include "common.h"
#include "decoder-ogg-stream.h"
#include "stream.h"

// --- Ogg callbacks -----------------------------------------------------------

static size_t callback_read(void *ptr, size_t size, size_t nmemb, void *datasource)
{
  cNetStream *nstr=(cNetStream*)datasource;
  unsigned char *sdata;
  unsigned long slen=0;
  // Read in loop until we either get data or function "Stream" fails
  do {
    if(!nstr->Stream(sdata,slen)) {
      d(printf("oggstream-callback-read: EOF?\n"))
      return 0;
      }
    } while(slen==0);

  size_t read_size=size*nmemb;
  if(slen>read_size) {
    // If someone ever gets this message, buffer handling has to be improved...
    d(printf("oggstream-callback-read: buffer size too small...\n"))
    slen=read_size;
    }
  memcpy(ptr,sdata,slen);
  return slen/size;
}

static int callback_close(void *datasource)
{
  cNetStream *nstr=(cNetStream*)datasource;
  nstr->Close();
  return 0;
}

static const ov_callbacks callbacks = {
  callback_read,
  NULL,
  callback_close,
  NULL
  };

// --- cNetOggFile -------------------------------------------------------------

cNetOggFile::cNetOggFile(const char *Filename)
:cOggFile(Filename)
{
  nstr=new cNetStream(Filename);
}

bool cNetOggFile::Open(bool log)
{
  if(opened) return true;
  if(!nstr->Open(log)) return false;
  int r=ov_open_callbacks(nstr,&vf,NULL,0,callbacks);
  if(!r) opened=true;
  else {
    nstr->Close();
    if(log) Error("open",r);
    }
  return opened;
}

// --- cNetOggInfo -------------------------------------------------------------

cNetOggInfo::cNetOggInfo(cNetOggFile *File)
:cOggInfo(File)
{
  nfile=File;
  nstr=nfile->nstr;
}

bool cNetOggInfo::DoScan(bool KeepOpen)
{
  Clear();
  IcyInfo();
  if(!Title) FakeTitle(nstr->Filename);
  Total=0;
  ChMode=3;
  DecoderID=DEC_OGGS;
  InfoDone();
  return true;
}

void cNetOggInfo::InfoHook()
{
  if(nstr->IcyChanged()) IcyInfo();
  vorbis_info *vi=ov_info(&nfile->vf,-1);
  if(!vi) return;
  Channels=vi->channels;
  ChMode=Channels>1 ? 3:0;
  SampleFreq=vi->rate;
  if(vi->bitrate_upper>0 && vi->bitrate_lower>0) {
    Bitrate=vi->bitrate_lower;
    MaxBitrate=vi->bitrate_upper;
    }
  else
    Bitrate=vi->bitrate_nominal;

  Total=(int)ov_time_total(&nfile->vf,-1);
  Frames=-1;
}

void cNetOggInfo::IcyInfo(void)
{
  const char *t=nstr->IcyTitle();
  const char *a;
  if(t) {
    a=nstr->IcyName();
    if(!a) a=nstr->IcyUrl();
    }
  else {
    t=nstr->IcyName();
    a=nstr->IcyUrl();
    }
  if(t && (!Title || strcmp(t,Title))) {
    free(Title);
    Title=strdup(t);
    }
  if(a && (!Album || strcmp(a,Album))) {
    free(Album);
    Album=strdup(a);
    }
}

// --- cOggStreamDecoder -------------------------------------------------------

cOggStreamDecoder::cOggStreamDecoder(const char *Filename)
:cOggDecoder(Filename,false)
{
  nfile=new cNetOggFile(Filename);
  file=nfile;
  info=new cNetOggInfo(nfile);
}
