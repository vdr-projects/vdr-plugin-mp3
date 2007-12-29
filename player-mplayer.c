/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2007 Stefan Huelswitt <s.huelswitt@gmx.de>
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

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <math.h>
#include <locale.h>

#include <vdr/device.h>
#include <vdr/videodir.h>

#include "common.h"
#include "data.h"
#include "player-mplayer.h"
#include "setup-mplayer.h"

//#define DEBUG_SLAVE

#define MPLAYER_VOL_STEP 3.0

const char *MPlayerCmd = "mplayer.sh";
int MPlayerAid=-1;
const char *globalResumeDir = 0;

// -- cMPlayerStatus -----------------------------------------------------------

cMPlayerStatus *status;

cMPlayerStatus::cMPlayerStatus(void)
{
  mute=changed=false;
  volume=0;
}

bool cMPlayerStatus::GetVolume(int &Volume, bool &Mute)
{
  Lock();
  bool r=changed;
  // convert according cDvbDevice::SetVolumeDevice();
  // take into account that VDR does non-linear changes, while
  // MPlayer does linear volume changes.
  Volume=((2*volume-volume*volume/255)*100+128)/256;
  Mute=mute;
  changed=false;
  Unlock();
  return r;
}

void cMPlayerStatus::SetVolume(int Volume, bool Absolute)
{
  Lock();
  if(Absolute && Volume==0) mute=true;
  else {
#if APIVERSNUM>=10401
#if APIVERSNUM==10401
#warning Caution! This code does not work with VDR 1.4.1 and 1.4.1-1. You can ignore this warning if you are using VDR 1.4.1-2 or later.
#endif
    if(!Absolute)
      volume+=Volume;
    else
#endif
      volume=Volume;
    if(volume>0) mute=false;
    }
  d(printf("status: volume=%d mute=%d\n",volume,mute))
  changed=true;
  Unlock();
}

// --- cResumeEntry ------------------------------------------------------------

class cResumeEntry : public cListObject {
public:
  char *name;
  float pos;
  //
  cResumeEntry(void);
  ~cResumeEntry();
  };

cResumeEntry::cResumeEntry(void)
{
  name=0;
}

cResumeEntry::~cResumeEntry()
{
  free(name);
}

// --- cMPlayerResume ----------------------------------------------------------

#define RESUME_FILE ".mplayer.resume"
#define GLOBAL_RESUME_FILE "global.mplayer.resume"

class cMPlayerResume : public cList<cResumeEntry> {
private:
  char *resfile;
  bool modified, global;
  cFileObj *resobj;
  //
  bool OpenResume(const cFileObj *file);
  bool SaveResume(void);
  void Purge(void);
  cResumeEntry *FindResume(const cFileObj *file);
public:
  cMPlayerResume(void);
  ~cMPlayerResume();
  void SetResume(const cFileObj *file, float pos);
  bool GetResume(const cFileObj *file, float &pos);
  };

cMPlayerResume::cMPlayerResume(void)
{
  resfile=0; resobj=0;
}

cMPlayerResume::~cMPlayerResume()
{
  SaveResume();
  free(resfile);
  delete resobj;
}

void cMPlayerResume::SetResume(const cFileObj *file, float pos)
{
  if(pos<0.001) pos=0.0;
  else if(pos>99.0) pos=99.0;
  cResumeEntry *re;
  if(OpenResume(file) && (re=FindResume(file))) {
    d(printf("resume: setting resume %f (update)\n",pos))
    }
  else {
    re=new cResumeEntry;
    re->name=strdup(global ? file->FullPath() : file->Name());
    Add(re);
    d(printf("resume: setting resume %f (new)\n",pos))
    }
  re->pos=pos;
  modified=true;
}

bool cMPlayerResume::GetResume(const cFileObj *file, float &pos)
{
  cResumeEntry *re;
  if(OpenResume(file) && (re=FindResume(file))) {
    pos=re->pos;
    return true;
    }
  return false;
}

bool cMPlayerResume::OpenResume(const cFileObj *file)
{
  if(!resfile) {
    Clear();
    modified=global=false;
    free(resfile); resfile=0;
    delete resobj; resobj=new cFileObj(file);
    char *s;
    asprintf(&s,file->Subdir() ? "%s/%s":"%s",file->Source()->BaseDir(),file->Subdir());
    if(MPlayerSetup.ResumeMode==1 || 
       (access(s,W_OK) && (errno==EACCES || errno==EROFS))) {
      global=true;
      resfile=AddPath(globalResumeDir?globalResumeDir:VideoDirectory,GLOBAL_RESUME_FILE);
      d(printf("resume: using global file\n"))
      }
    else {
      resfile=AddPath(s,RESUME_FILE);
      }
    free(s);
    d(printf("resume: resume file is '%s'\n",resfile))
    FILE *f=fopen(resfile,"r");
    if(f) {
      d(printf("resume: successfully opened resume file\n"))
      char line[768];
      while(fgets(line,sizeof(line),f)) {
        char name[512];
        float p;
        if(sscanf(line,"%f:%511[^\n]",&p,name)==2) {
          cResumeEntry *re=new cResumeEntry;
          re->name=strdup(name);
          re->pos=p;
          Add(re);
          }
        }
      fclose(f);
      return true;
      }
    else {
      d(printf("resume: assuming empty resume file\n"))
      return false;
      }
    }
  return true;
}

bool cMPlayerResume::SaveResume(void)
{
  if(resfile && modified) {
    Purge();
    d(printf("resume: saving resume file\n"))
    cSafeFile f(resfile);
    if(f.Open()) {
      for(cResumeEntry *re=First(); re; re=Next(re))
        fprintf(f,"%06.2f:%s\n",re->pos,re->name);
      f.Close();
      return true;
      }
    else
      d(printf("resume: failed to save resume file\n"))
    }
  return false;
}

void cMPlayerResume::Purge(void)
{
  d(printf("resume: purging from resume file\n"))
  for(cResumeEntry *re=First(); re;) {
    bool del=false;
    if(re->pos<1.0 || re->pos>99.0) {
      del=true;
      d(printf("resume: purging due to position: %s\n",re->name))
      }
    else if(!global) {
      resobj->SetName(re->name);
      if(access(resobj->FullPath(),F_OK)<0) {
        del=true;
        d(printf("resume: purging due to access: %s\n",re->name))
        }
      }
    if(del) {
      cResumeEntry *n=Next(re);
      Del(re);
      modified=true;
      re=n;
      }
    else
      re=Next(re);
    }
}

cResumeEntry *cMPlayerResume::FindResume(const cFileObj *file)
{
 if(resfile) {
   d(printf("resume: searching resume  position for '%s'\n",file->Name()))
   const char *s=global ? file->FullPath() : file->Name();
   for(cResumeEntry *re=First(); re; re=Next(re))
     if(!strcasecmp(re->name,s)) {
       d(printf("resume: found resume position %.1f%%\n",re->pos))
       return re;
       }
   }
 d(printf("resume: no resume position found\n"))
 return 0;
}

// --- cMPlayerPlayer ----------------------------------------------------------

cMPlayerPlayer::cMPlayerPlayer(const cFileObj *File, bool Rewind)
:cPlayer(pmExtern_THIS_SHOULD_BE_AVOIDED)
{
  started=slave=brokenPipe=false; run=true; pid=-1; pipefl=0;
  playMode=pmPlay; index=saveIndex=total=-1; nextTime=nextPos=0;
  currentName=0;
  file=new cFileObj(File);
  rewind=Rewind;
  resume=MPlayerSetup.ResumeMode ? new cMPlayerResume : 0;
}

cMPlayerPlayer::~cMPlayerPlayer()
{
  Detach();
  ClosePipe();
  delete file;
  delete resume;
  free(currentName);
}

void cMPlayerPlayer::ClosePipe(void)
{
  if(pipefl&1) close(inpipe[0]);
  if(pipefl&2) close(inpipe[1]);
  if(pipefl&4) close(outpipe[0]);
  if(pipefl&8) close(outpipe[1]);
  pipefl=0;
}

void cMPlayerPlayer::Activate(bool On)
{
  if(On) {
    if(file && !started) {
      if(Fork()) started=true;
      }
    }
  else if(started) {
    run=false;
    if(Active()) {
      if(slave) {
        Play(); // MPlayer ignores "quit" while paused
        MPlayerControl("quit");
        int until=time_ms()+3000; // wait some time until MPlayer is gone
        d(printf("mplayer: waiting for child exit"))
        while(Active()) {
          if(time_ms()>until) {
            kill(pid,SIGKILL); // kill it anyways
            d(printf(" SIGKILL"))
            break;
            }
          SLEEP(250);
          d(printf(".")) d(fflush(stdout))
          }
        d(printf("\n"))
        }
      else {
        kill(pid,SIGTERM);
        d(printf("mplayer: waiting for child exit (non-slave)\n"))
        }
      waitpid(pid,0,0);
      }
    ClosePipe();
    Cancel(2);
    started=slave=false;
    }
}

bool cMPlayerPlayer::Active(void)
{
  return waitpid(pid,0,WNOHANG)==0;
}

bool cMPlayerPlayer::Fork(void)
{
  if(MPlayerSetup.SlaveMode) {
    if(pipe(inpipe)==-1) {
      esyslog("ERROR: pipe failed for inpipe: (%d) %s",errno,strerror(errno));
      return false;
      }
    pipefl|=1+2;
    if(pipe(outpipe)==-1) {
      esyslog("ERROR: pipe failed for outpipe: (%d) %s",errno,strerror(errno));
      return false;
      }
    pipefl|=4+8;
    brokenPipe=false;
    }

  pid=fork();
  if(pid==-1) {
    esyslog("ERROR: fork failed: (%d) %s",errno,strerror(errno));
    return false;
    }
  if(pid==0) { // child
    dsyslog("mplayer: mplayer child started (pid=%d)", getpid());

    if(MPlayerSetup.SlaveMode) {
      if(dup2(inpipe[0],STDIN_FILENO)<0 ||
         dup2(outpipe[1],STDOUT_FILENO)<0 ||
         dup2(outpipe[1],STDERR_FILENO)<0) {
        esyslog("ERROR: dup2() failed in MPlayer child: (%d) %s",errno,strerror(errno));
        exit(127);
        }
      }
    else {
      int nfd=open("/dev/null",O_RDONLY);
      if(nfd<0 || dup2(nfd,STDIN_FILENO)<0)
        esyslog("ERROR: redirect of STDIN failed in MPlayer child: (%d) %s",errno,strerror(errno));
      }
    for(int i=getdtablesize()-1; i>STDERR_FILENO; i--) close(i);

    char cmd[64+PATH_MAX*2], aid[20];
    char *fname=Quote(file->FullPath());
    if(MPlayerAid>=0) snprintf(aid,sizeof(aid)," AID %d",MPlayerAid);
    else aid[0]=0;
    snprintf(cmd,sizeof(cmd),"%s \"%s\" %s%s",MPlayerCmd,fname,MPlayerSetup.SlaveMode?"SLAVE":"",aid);
    free(fname);
    execle("/bin/sh","sh","-c",cmd,(char *)0,environ);
    esyslog("ERROR: exec failed for %s: (%d) %s",cmd,errno,strerror(errno));
    exit(127);
    }

  if(MPlayerSetup.SlaveMode) {
    close(inpipe[0]); pipefl&=~1;
    close(outpipe[1]); pipefl&=~8;
    fcntl(outpipe[0],F_SETFL,O_NONBLOCK);
    run=slave=true;
    mpVolume=100; // MPlayer startup defaults
    mpMute=false;
    Start();
    }
  return true;
}

#define BSIZE    1024
#define TIME_INT 20
#define POS_INT  1

void cMPlayerPlayer::Action(void)
{
  dsyslog("mplayer: player thread started (pid=%d)", getpid());

  // set locale for correct parsing of MPlayer output.
  // I don't know if this affects other parts of VDR.
  const char * const oldLocale=setlocale(LC_NUMERIC,"C");

  pollfd pfd[1];
  pfd[0].fd=outpipe[0];
  pfd[0].events=POLLIN;

  float curPos=-1.0, resPos=-1.0;
  if(resume && !rewind) resume->GetResume(file,resPos);

  char buff[BSIZE+2]; // additional space for fake newline
  int c=0;
  bool force=true, slavePatch=false, trustedTotal=false, playBack=false;
  while(run) {
    if(playMode==pmPlay && playBack) {
      int t=time(0);
      if(t>=nextTime) {
        MPlayerControl("get_time_length");
        nextTime=t+(total>0 ? TIME_INT : POS_INT);
        }
      if(t>=nextPos) {
        if(!slavePatch) MPlayerControl("get_percent_pos");
        nextPos=t+POS_INT;
        }
      }

    poll(pfd,1,300);
    int r=read(outpipe[0],buff+c,BSIZE-c);
    if(r>0) c+=r;
    if(c>0) {
      buff[c]=0; // make sure buffer is NULL terminated
      char *p;
      do {
        p=strpbrk(buff,"\n\r");
        if(!p && c==BSIZE) { // Full buffer, but no newline found.
          p=&buff[c];        // Have to fake one.
          buff[c]='\n'; c++; buff[c]=0;
          }
        if(p) {
#ifdef DEBUG
          char cc=*p;
#endif
          *p++=0;
          float ftime=-1.0, fpos=-1.0;
          int itime;
          if(strncmp(buff,"Starting playback",17)==0 ||
             strncmp(buff,"Starte Wiedergabe",17)==0) {
            if(!playBack) {
              playBack=true;
              nextTime=nextPos=0;
              d(printf("PLAYBACK STARTED\n"))
              if(resPos>=0.0) {
                if(!currentName ||
                   !strcmp(currentName,file->FullPath()) ||
                   !strcmp(currentName,file->Path()))
                  MPlayerControl("seek %.1f 1",resPos);
                else
                  d(printf("mplayer: no resume, seems to be playlist\n"))
                }
              }
            }
          else if(strncmp(buff,"Playing ",8)==0 ||
                  strncmp(buff,"Spiele ",7)==0) {
            nextTime=nextPos=0;
            index=saveIndex=total=-1;
            trustedTotal=false;
            LOCK_THREAD;
            free(currentName);
            currentName=strdup(::index(buff,' ')+1);
            if(currentName[0]) {
              int l=strlen(currentName);
              if(currentName[l-1]=='.') currentName[l-1]=0; // skip trailing dot
              }
            d(printf("PLAYING %s\n",currentName))
            }
          else if(sscanf(buff,"ANS_LENGTH=%d",&itime)==1) {
            if(itime>0) {
              total=SecondsToFrames(itime);
              trustedTotal=true;
#ifdef DEBUG_SLAVE
              printf("sl: ANS_LENGTH=%s (%s)\n",IndexToHMSF(total),buff);
#endif
              }
            }
          else if(sscanf(buff,"ANS_PERCENT_POSITION=%d",&itime)==1) {
            if(itime>0) {
              curPos=itime;
              if(total>=0) {
                index=total*itime/100;
#ifdef DEBUG_SLAVE
                printf("sl: ANS_PERCENT_POS=%s (%s)\n",IndexToHMSF(index),buff);
#endif
                }
              }
            }
          else if(sscanf(buff,"SLAVE: time=%f position=%f",&ftime,&fpos)==2) {
            curPos=fpos;
            const float fr=(float)SecondsToFrames(1);
            itime=(int)(ftime*fr);
            if(saveIndex<0 || itime>saveIndex) { // prevent index jump-back
              saveIndex=index=itime;
              if(!trustedTotal) total=(int)(ftime*fr*100.0/fpos);
#ifdef DEBUG_SLAVE
              printf("sl: SLAVE=%s/%s [%d] (%s)\n",IndexToHMSF(index),IndexToHMSF(total),trustedTotal,buff);
#endif
              }
            slavePatch=playBack=true;
            }
#ifdef DEBUG
          else printf("%s%c",buff,cc);
#endif
          c-=(p-buff);
          memmove(buff,p,c+1);
          }
        } while(c>0 && p);
      }
    if(playBack) {
      SetMPlayerVolume(force);
      force=false;
      }
    }

  if(resume && curPos>=0.0) resume->SetResume(file,curPos);

  // restore old locale
  if(oldLocale) setlocale(LC_NUMERIC,oldLocale);

  dsyslog("mplayer: player thread ended (pid=%d)", getpid());
}

void cMPlayerPlayer::SetMPlayerVolume(bool force)
{
  int volume;
  bool mute;
  Lock();
  if(status->GetVolume(volume,mute) || force) {
    if(mute) {
      if(!mpMute) { MPlayerControl("mute"); mpMute=true; }
      }
    else {
      if(mpMute) { MPlayerControl("mute"); mpMute=false; }
      if(volume!=mpVolume) {
        MPlayerControl("volume %d 1",volume);
        mpVolume=volume;
        }
      }
    d(printf("mplayer: volume=%d mpVolume=%d mpMute=%d\n",volume,mpVolume,mpMute))
    }
  Unlock();
}

void cMPlayerPlayer::MPlayerControl(const char *format, ...)
{
  if(slave) {
    va_list ap;
    va_start(ap,format);
    char *buff=0;
    vasprintf(&buff,format,ap);
    Lock();
    // check for writeable pipe i.e. prevent broken pipe signal
    if(!brokenPipe) {
      struct pollfd pfd;
      pfd.fd=inpipe[1]; pfd.events=POLLOUT; pfd.revents=0;
      int r=poll(&pfd,1,50);
      if(r>0) {
        if(pfd.revents & ~POLLOUT) {
          d(printf("mplayer: %s%s%s%sin MPlayerControl\n",pfd.revents&POLLOUT?"POLLOUT ":"",pfd.revents&POLLERR?"POLLERR ":"",pfd.revents&POLLHUP?"POLLHUP ":"",pfd.revents&POLLNVAL?"POLLNVAL ":""))
          brokenPipe=true;
          }
        else if(pfd.revents & POLLOUT) {
          r=write(inpipe[1],buff,strlen(buff));
          if(r<0) {
            d(printf("mplayer: pipe write(1) failed: %s\n",strerror(errno)))
            brokenPipe=true;
            }
          else {
            r=write(inpipe[1],"\n",1);
            if(r<0) {
              d(printf("mplayer: pipe write(2) failed: %s\n",strerror(errno)))
              brokenPipe=true;
              }
            }
          }
        }
      else if(r==0) d(printf("mplayer: poll timed out in MPlayerControl (hugh?)\n"))
      else d(printf("mplayer: poll failed in MPlayerControl: %s\n",strerror(errno)))
      }
    else d(printf("mplayer: cmd pipe is broken\n"))
    Unlock();
    d(printf("mplayer: slave cmd: %s\n",buff))
    free(buff);
    va_end(ap);
    }
}

void cMPlayerPlayer::Pause(void)
{
  if(slave) {
    if(playMode==pmPaused) Play();
    else if(playMode==pmPlay) {
      playMode=pmPaused;
      MPlayerControl("pause");
      }
    }
}

void cMPlayerPlayer::Play(void)
{
  if(slave) {
    if(playMode==pmPaused) {
      playMode=pmPlay;
      MPlayerControl("pause");
      }
    }
}

void cMPlayerPlayer::Goto(int Index, bool percent, bool still)
{
  if(slave) {
    if(playMode==pmPaused) Play();
    if(percent) MPlayerControl("seek %d 1",Index);
    else        MPlayerControl("seek %+d 0",Index-(index/SecondsToFrames(1)));
    if(still) Pause();
    saveIndex=-1;
    }
}

void cMPlayerPlayer::SkipSeconds(int secs)
{
  if(slave) {
    bool p=false;
    if(playMode==pmPaused) { Play(); p=true; }
    MPlayerControl("seek %+d 0",secs);
    if(p) Pause();
    saveIndex=-1;
    }
}

void cMPlayerPlayer::KeyCmd(const char *cmd)
{
  if(slave) MPlayerControl(cmd);
}

bool cMPlayerPlayer::GetIndex(int &Current, int &Total, bool SnapToIFrame)
{
  Current=index; Total=total;
  return true;
}

bool cMPlayerPlayer::GetReplayMode(bool &Play, bool &Forward, int &Speed)
{
  Play=(playMode==pmPlay);
  Forward=true;
  Speed=-1;
  return true;
}

char *cMPlayerPlayer::GetCurrentName(void)
{
  LOCK_THREAD;
  return currentName ? strdup(currentName) : 0;
}

