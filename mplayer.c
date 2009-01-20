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

#include <getopt.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>

#include "common.h"

#include <vdr/plugin.h>
#include <vdr/player.h>
#include <vdr/status.h>
#include <vdr/font.h>
#include <vdr/osdbase.h>
#include <vdr/menuitems.h>
#ifdef HAVE_BEAUTYPATCH
#include <vdr/fontsym.h>
#endif
#if APIVERSNUM >= 10307
#include <vdr/skins.h>
#endif
#if APIVERSNUM >= 10332
#include <vdr/remote.h>
#endif

#if APIVERSNUM > 10307
#include <vdr/menu.h>
#elif APIVERSNUM == 10307
class cMenuText : public cOsdMenu {
private:
  char *text;
public:
  cMenuText(const char *Title, const char *Text, eDvbFont Font = fontOsd);
  virtual ~cMenuText();
  void SetText(const char *Text);
  virtual void Display(void);
  virtual eOSState ProcessKey(eKeys Key);
  };
#else
class cMenuText : public cOsdMenu {
public:
  cMenuText(const char *Title, const char *Text, eDvbFont Font = fontOsd);
  virtual eOSState ProcessKey(eKeys Key);
  };
#endif

#include "setup.h"
#include "setup-mplayer.h"
#include "menu.h"
#include "player-mplayer.h"
#include "data.h"
#include "data-src.h"
#include "i18n.h"
#include "version.h"
#include "service.h"

const char *sourcesSub=0;
cFileSources MPlaySources;

static const char *plugin_name=0;
const char *i18n_name=0;

// --- cMenuSetupMPlayer --------------------------------------------------------

class cMenuSetupMPlayer : public cMenuSetupPage {
private:
  cMPlayerSetup data;
  const char *res[3];
protected:
  virtual void Store(void);
public:
  cMenuSetupMPlayer(void);
  };

cMenuSetupMPlayer::cMenuSetupMPlayer(void)
{
  data=MPlayerSetup;
  SetSection(tr("MPlayer"));
  Add(new cMenuEditBoolItem(tr("Setup.MPlayer$Control mode"),  &data.SlaveMode, tr("Traditional"), tr("Slave")));
#if APIVERSNUM < 10307
  Add(new cMenuEditIntItem( tr("Setup.MPlayer$OSD position"),  &data.OsdPos, 0, 6));
#endif
  res[0]=tr("disabled");
  res[1]=tr("global only");
  res[2]=tr("local first");
  Add(new cMenuEditStraItem(tr("Setup.MPlayer$Resume mode"),   &data.ResumeMode, 3, res));
  Add(new cMenuEditBoolItem(tr("Hide mainmenu entry"),         &data.HideMainMenu));
  for(int i=0; i<10; i++) {
    char name[32];
    snprintf(name,sizeof(name),"%s %d",tr("Setup.MPlayer$Slave command key"),i);
    static const char allowed[] = { "abcdefghijklmnopqrstuvwxyz0123456789!\"§$%&/()=?{}[]\\+*~#',;.:-_<>|@´`^°" };
    Add(new cMenuEditStrItem(name, data.KeyCmd[i],MAX_KEYCMD,allowed));
    }
}

void cMenuSetupMPlayer::Store(void)
{
  MPlayerSetup=data;
  SetupStore("ControlMode", MPlayerSetup.SlaveMode);
  SetupStore("HideMainMenu",MPlayerSetup.HideMainMenu);
  SetupStore("ResumeMode",  MPlayerSetup.ResumeMode);
#if APIVERSNUM < 10307
  SetupStore("OsdPos",      MPlayerSetup.OsdPos);
#endif
  for(int i=0; i<10; i++) {
    char name[16];
    snprintf(name,sizeof(name),"KeyCmd%d",i);
    SetupStore(name,MPlayerSetup.KeyCmd[i]);
    }
}

// --- cMPlayerControl ---------------------------------------------------------

class cMPlayerControl : public cControl {
private:
  static cFileObj *file;
  static bool rewind;
  cMPlayerPlayer *player;
#if APIVERSNUM >= 10307
  cSkinDisplayReplay *display;
#endif
  bool visible, modeOnly, haveBeauty;
  time_t timeoutShow;
  int lastCurrent, lastTotal;
  char *lastReplayMsg;
  //
  bool jumpactive, jumphide, jumpmode;
  int jumpval;
  //
  void Stop(void);
  void ShowTimed(int Seconds=0);
  void DisplayAtBottom(const char *s);
  void ShowProgress(void);
  void ShowMode(void);
  void ShowTitle(void);
  void Jump(void);
  void JumpProcess(eKeys Key);
  void JumpDisplay(void);
public:
  cMPlayerControl(void);
  virtual ~cMPlayerControl();
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Show(void) { ShowTimed(); }
  virtual void Hide(void);
  static void SetFile(const cFileObj *File, bool Rewind);
  };

cFileObj *cMPlayerControl::file=0;
bool cMPlayerControl::rewind=false;

cMPlayerControl::cMPlayerControl(void)
:cControl(player=new cMPlayerPlayer(file,rewind))
{
  visible=modeOnly=jumpactive=haveBeauty=false;
  lastReplayMsg=0;
#if APIVERSNUM >= 10307
  display=0;
#else
#ifdef HAVE_BEAUTYPATCH
#if APIVERSNUM >= 10300
  const cFont *sym=cFont::GetFont(fontSym);
  const cFont *osd=cFont::GetFont(fontOsd);
  const cFont::tCharData *symD=sym->CharData(32);
  const cFont::tCharData *osdD=osd->CharData(32);
#else //APIVERSNUM >= 10300
  cFont *sym=new cFont(fontSym);
  cFont *osd=new cFont(fontOsd);
  const cFont::tCharData *symD=sym->CharData(32);
  const cFont::tCharData *osdD=osd->CharData(32);
  delete sym;
  delete osd;
#endif //APIVERSNUM >= 10300
  if(symD != osdD) haveBeauty=true;
  d(printf("mplayer: beauty patch %sdetected\n",haveBeauty?"":"NOT "))
#endif //HAVE_BEAUTYPATCH
#endif //APIVERSNUM >= 10307
  ShowTitle();
}

cMPlayerControl::~cMPlayerControl()
{
  Stop();
#if APIVERSNUM >= 10338
  cStatus::MsgReplaying(this,0,0,false);
#else
  cStatus::MsgReplaying(this, NULL);
#endif
  free(lastReplayMsg);
}

void cMPlayerControl::SetFile(const cFileObj *File, bool Rewind)
{
  delete file;
  file=File ? new cFileObj(File) : 0;
  rewind=Rewind;
}

void cMPlayerControl::Stop(void)
{
  delete player; player=0;
}

void cMPlayerControl::ShowTimed(int Seconds)
{
  if(modeOnly) Hide();
  if(!visible) {
    ShowProgress();
    timeoutShow = Seconds>0 ? time(0)+Seconds : 0;
    }
}

void cMPlayerControl::Hide(void)
{
  if(visible) {
#if APIVERSNUM >= 10307
    delete display; display=0;
#else
    Interface->Close();
#endif
    visible=modeOnly=false;
#if APIVERSNUM >= 10500
    SetNeedsFastResponse(false);
#else
    needsFastResponse=false;
#endif
    }
}

void cMPlayerControl::DisplayAtBottom(const char *s)
{
#if APIVERSNUM < 10307
  const int p=modeOnly ? 0 : 2;
  if(s) {
    const int d=max(Width()-cOsd::WidthInCells(s),0) / 2;
    if(modeOnly) Interface->Fill(0, p, Interface->Width(), 1, clrTransparent);
    Interface->Write(d, p, s);
    }
  else
    Interface->Fill(12, p, Width() - 22, 1, clrBackground);
#endif
}

void cMPlayerControl::ShowTitle(void)
{
  const char *path=0;
  bool release=true;
  if(player) path=player->GetCurrentName();
  if(!path) {
    path=file->FullPath();
    release=false;
    }
  if(path) {
    const char *name=rindex(path,'/');
    if(name) name++; else name=path;
    if(!lastReplayMsg || strcmp(lastReplayMsg,path)) {
#if APIVERSNUM >= 10338
      cStatus::MsgReplaying(this,name,path,true);
#else
      cStatus::MsgReplaying(this,path);
#endif
      free(lastReplayMsg);
      lastReplayMsg=strdup(path);
      }
    if(visible) {
#if APIVERSNUM >= 10307
      if(display) display->SetTitle(name);
#else
      int n=strlen(name);
      if(n>Width()) {
        n=n-Width()+4; if(n<0) n=0;
        char str[72];
        snprintf(str,sizeof(str),"... %s",name+n);
        Interface->Write(0,0,str);
        }
      else Interface->Write(0,0,name);
#endif
      }
    }
  if(release) free((void *)path);
}

void cMPlayerControl::ShowProgress(void)
{
  int Current, Total;

  if(GetIndex(Current,Total) && Total>0) {
    bool flush=false;
    if(!visible) {
#if APIVERSNUM >= 10307
      display=Skins.Current()->DisplayReplay(false);
#else
      Interface->Open(Setup.OSDwidth,-MPlayerSetup.OsdPos-3);
      Interface->Clear();
      if(MPlayerSetup.OsdPos>0) Interface->Fill(0,3,Interface->Width(),MPlayerSetup.OsdPos,clrTransparent);
#endif
      visible=true; modeOnly=false;
#if APIVERSNUM >= 10500
      SetNeedsFastResponse(true);
#else
      needsFastResponse=true;
#endif
      lastCurrent=lastTotal=-1;
      flush=true;
      }

    if(abs(Current-lastCurrent)>12) {
#if APIVERSNUM >= 10307
      if(Total>0) display->SetProgress(Current, Total);
      display->SetCurrent(IndexToHMSF(Current));
      display->SetTotal(IndexToHMSF(Total));
      bool Play, Forward;
      int Speed;
      if(GetReplayMode(Play,Forward,Speed)) 
        display->SetMode(Play, Forward, Speed);
#else
      cProgressBar ProgressBar(Width() * cOsd::CellWidth(), cOsd::LineHeight(), Current, Total);
      Interface->SetBitmap(0, cOsd::LineHeight(), ProgressBar);
      Interface->Write(0,2,IndexToHMSF(Current));
      Interface->Write(-7,2,IndexToHMSF(Total));
#endif
      ShowTitle();
      flush=true;
      lastCurrent=Current; lastTotal=Total;
      }
    if(flush) 
#if APIVERSNUM >= 10307
      Skins.Flush();
#else
      Interface->Flush();
#endif
    ShowMode();
    }
}

#if APIVERSNUM < 10307
#ifdef HAVE_BEAUTYPATCH
int forwSym[] = { FSYM_FORW,FSYM_FORW1,FSYM_FORW2,FSYM_FORW3 };
int backSym[] = { FSYM_BACK,FSYM_BACK1,FSYM_BACK2,FSYM_BACK3 };
#endif
#endif

void cMPlayerControl::ShowMode(void)
{
  if(Setup.ShowReplayMode && !jumpactive) {
    bool Play, Forward;
    int Speed;
    if(GetReplayMode(Play, Forward, Speed)) {
       bool NormalPlay = (Play && Speed == -1);

       if(!visible) {
         if(NormalPlay) return;
#if APIVERSNUM >= 10307
         display = Skins.Current()->DisplayReplay(true);
#else
         Interface->Open(0,-MPlayerSetup.OsdPos-1);
#endif
         visible=modeOnly=true;
         }

       if(modeOnly && !timeoutShow && NormalPlay) timeoutShow=time(0)+SELECTHIDE_TIMEOUT;

#if APIVERSNUM >= 10307
       display->SetMode(Play, Forward, Speed);
#else
       char buf[16];
       eDvbFont OldFont;
#ifdef HAVE_BEAUTYPATCH
       if(haveBeauty) {
         int i=0;
         if(!(Width()&1)) buf[i++]=' ';
         buf[i]=FSYM_EMPTY; if(Speed>=0 && !Forward) buf[i]=backSym[Speed];
         i++;
         buf[i++]=Play?(Speed==-1?FSYM_PLAY:FSYM_EMPTY):FSYM_PAUSE;
         buf[i]=FSYM_EMPTY; if(Speed>=0 && Forward) buf[i]=forwSym[Speed];
         i++;
         if(!(Width()&1)) buf[i++]=' ';
         buf[i]=0;
         OldFont = Interface->SetFont(fontSym);
         }
       else {
#endif //HAVE_BEAUTYPATCH
         const char *Mode;
         if (Speed == -1) Mode = Play    ? "  >  " : " ||  ";
         else if (Play)   Mode = Forward ? " X>> " : " <<X ";
         else             Mode = Forward ? " X|> " : " <|X ";
         strn0cpy(buf, Mode, sizeof(buf));
         char *p = strchr(buf, 'X');
         if(p) *p = Speed > 0 ? '1' + Speed - 1 : ' ';
         OldFont = Interface->SetFont(fontFix);
#ifdef HAVE_BEAUTYPATCH
         }
#endif //HAVE_BEAUTYPATCH
       DisplayAtBottom(buf);
       Interface->SetFont(OldFont);
#endif //APIVERSNUM >= 10307
       }
    }
}

void cMPlayerControl::JumpDisplay(void)
{
  char buf[64];
  const char *j=trVDR("Jump: "), u=jumpmode?'%':'m';
  if(!jumpval) sprintf(buf,"%s- %c",  j,u);
  else         sprintf(buf,"%s%d- %c",j,jumpval,u);
#if APIVERSNUM >= 10307
  display->SetJump(buf);
#else
  DisplayAtBottom(buf);
#endif
}

void cMPlayerControl::JumpProcess(eKeys Key)
{
  const int n=Key-k0;
  switch (Key) {
    case k0 ... k9:
      {
      const int max=jumpmode?100:lastTotal;
      if(jumpval*10+n <= max) jumpval=jumpval*10+n;
      JumpDisplay();
      }
      break;
    case kBlue:
      jumpmode=!jumpmode; jumpval=0;
      DisplayAtBottom(0); JumpDisplay();
      break;
    case kPlay:
    case kUp:
      player->Goto(jumpval*(jumpmode?1:60),jumpmode,false);
      jumpactive=false;
      break;
    case kFastRew:
    case kFastFwd:
    case kLeft:
    case kRight:
      if(!jumpmode) {
        player->SkipSeconds(jumpval*60 * ((Key==kLeft || Key==kFastRew) ? -1:1));
        jumpactive=false;
        }
      break;
    default:
      jumpactive=false;
      break;
    }

  if(!jumpactive) {
    if(jumphide) Hide();
    else 
#if APIVERSNUM >= 10307
      display->SetJump(0);
#else
      DisplayAtBottom(0);
#endif
    }
}

void cMPlayerControl::Jump(void)
{
  jumpval=0; jumphide=jumpmode=false;
  if(!visible) {
    ShowTimed(); if(!visible) return;
    jumphide=true;
    }
  JumpDisplay();
  jumpactive=true;
}

eOSState cMPlayerControl::ProcessKey(eKeys Key)
{
  if(!player->Active()) { Hide(); Stop(); return osEnd; }

  if(!player->SlaveMode()) {
    if(Key==kBlue) { Hide(); Stop(); return osEnd; }
    }
  else {
    if(visible) {
      if(timeoutShow && time(0)>timeoutShow) {
        Hide(); ShowMode();
        timeoutShow = 0;
        }
      else {
        if(modeOnly) ShowMode();
        else ShowProgress();
        }
      }
    else ShowTitle();

    if(jumpactive && Key != kNone) {
      JumpProcess(Key);
      return osContinue;
      }

    bool DoShowMode = true;
    switch (Key) {
      case kPlay:
      case kUp:      player->Play(); break;

      case kPause:
      case kDown:    player->Pause(); break;

      case kFastRew|k_Repeat:
      case kFastRew:
      case kLeft|k_Repeat:
      case kLeft:    player->SkipSeconds(-10); break;

      case kFastFwd|k_Repeat:
      case kFastFwd:
      case kRight|k_Repeat:
      case kRight:   player->SkipSeconds(10); break;

      case kRed:     Jump(); break;

      case kGreen|k_Repeat:                      // temporary use
      case kGreen:   player->SkipSeconds(-60); break;
      case kYellow|k_Repeat:
      case kYellow:  player->SkipSeconds(60); break;
  //    case kGreen|k_Repeat:                      // reserved for future use
  //    case kGreen:   player->SkipPrev(); break;
  //    case kYellow|k_Repeat:
  //    case kYellow:  player->SkipNext(); break;

      case kBack:
#if APIVERSNUM >= 10332
                     Hide();
                     cRemote::CallPlugin(plugin_name);
                     return osBack;
#endif
      case kStop:
      case kBlue:    Hide(); Stop(); return osEnd;

      default:
        DoShowMode = false;
        switch(Key) {
          case kOk: if(visible && !modeOnly) { Hide(); DoShowMode=true; }
                    else ShowTimed();
                    break;
#if APIVERSNUM >= 10318
          case kAudio:
                    player->KeyCmd("switch_audio");
                    break;
#endif
          case k0:
          case k1:
          case k2:
          case k3:
          case k4:
          case k5:
          case k6:
          case k7:
          case k8:
          case k9:  {
                    const char *cmd=MPlayerSetup.KeyCmd[Key-k0];
                    if(cmd[0]) player->KeyCmd(cmd);
                    }
                    break;
          default:  break;
          }
        break;
      }

    if(DoShowMode) ShowMode();
    }
  return osContinue;
}

// --- cMenuMPlayAid -----------------------------------------------------------

class cMenuMPlayAid : public cOsdMenu {
public:
  cMenuMPlayAid(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuMPlayAid::cMenuMPlayAid(void)
:cOsdMenu(tr("MPlayer Audio ID"),20)
{
  Add(new cMenuEditIntItem(tr("Audiostream ID"),&MPlayerAid,-1,255));
  Display();
}

eOSState cMenuMPlayAid::ProcessKey(eKeys Key)
{
  eOSState state=cOsdMenu::ProcessKey(Key);
  if(state==osUnknown) {
    switch(Key) {
      case kOk: state=osBack; break;
      default:  break;
      }
    }
  return state;
}

// --- cMenuMPlayBrowse ---------------------------------------------------------

class cMenuMPlayBrowse : public cMenuBrowse {
private:
  bool sourcing, aidedit;
  eOSState Source(bool second);
  eOSState Summary(void);
protected:
  virtual void SetButtons(void);
public:
  cMenuMPlayBrowse(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

static const char *excl_sum[] = { ".*","*.summary","*.txt","*.nfo",0 };

cMenuMPlayBrowse::cMenuMPlayBrowse(void)
:cMenuBrowse(MPlaySources.GetSource(),false,false,tr("MPlayer browser"),excl_sum)
{
  sourcing=aidedit=false;
  SetButtons();
}

void cMenuMPlayBrowse::SetButtons(void)
{
  static char blue[12];
  snprintf(blue,sizeof(blue),MPlayerAid>=0 ? "AID:%d" : "AID:def",MPlayerAid);
  SetHelp(trVDR(BUTTON"Play"), MPlayerSetup.ResumeMode ? trVDR(BUTTON"Rewind"):0, tr("Source"), blue);
  Display();
}

eOSState cMenuMPlayBrowse::Source(bool second)
{
  if(HasSubMenu()) return osContinue;

  if(!second) {
    sourcing=true;
    return AddSubMenu(new cMenuSource(&MPlaySources,tr("MPlayer source")));
    }
  sourcing=false;
  cFileSource *src=cMenuSource::GetSelected();
  if(src) {
    MPlaySources.SetSource(src);
    SetSource(src);
    NewDir(0);
    }
  return osContinue;
}

eOSState cMenuMPlayBrowse::Summary(void)
{
  cFileObj *item=CurrentItem();
  if(item && item->Type()==otFile) {
    static const char *exts[] = { ".summary",".txt",".nfo",0 };
    for(int i=0; exts[i]; i++) {
      char buff[4096];
      strn0cpy(buff,item->FullPath(),sizeof(buff)-20);
      char *e=&buff[strlen(buff)];
      strn0cpy(e,exts[i],20);
      int fd=open(buff,O_RDONLY);
      *e=0;
      if(fd<0 && (e=rindex(buff,'.'))) {
        strn0cpy(e,exts[i],20);
        fd=open(buff,O_RDONLY);
        }
      if(fd>=0) {
        int r=read(fd,buff,sizeof(buff)-1);
        close(fd);
        if(r>0) {
          buff[r]=0;
          return AddSubMenu(new cMenuText(tr("Summary"),buff));
          }
        }
      }
    }
  return osContinue;
}

eOSState cMenuMPlayBrowse::ProcessKey(eKeys Key)
{
  eOSState state=cOsdMenu::ProcessKey(Key);
  if(state==osContinue && !HasSubMenu()) {
    if(sourcing) return Source(true);
    if(aidedit) { aidedit=false; SetButtons(); }
    }
  bool rew=false;
  if(state==osUnknown) {
    switch(Key) {
      case kGreen:
        {
        cFileObj *item=CurrentItem();
        if(item && item->Type()==otFile) {
          lastselect=new cFileObj(item);
          state=osBack;
          rew=true;
          } 
        else state=osContinue;
        break;
        }
      case kYellow:
        state=Source(false);
        break;
      case kBlue:
        aidedit=true;
        state=AddSubMenu(new cMenuMPlayAid);
        break;
      case k0:
        state=Summary();
        break;
      default:
        break;
      }
    }
  if(state==osUnknown) state=cMenuBrowse::ProcessStdKey(Key,state);
  if(state==osBack && lastselect) {
    cMPlayerControl::SetFile(lastselect,rew);
    cControl::Launch(new cMPlayerControl);
    return osEnd;
    }
  return state;
}

// --- cPluginMPlayer ----------------------------------------------------------

static const char *DESCRIPTION    = trNOOP("Media replay via MPlayer");
static const char *MAINMENUENTRY  = "MPlayer";

class cPluginMPlayer : public cPlugin {
private:
#if APIVERSNUM >= 10330
  bool ExternalPlay(const char *path, bool test);
#endif
public:
  cPluginMPlayer(void);
  virtual ~cPluginMPlayer();
  virtual const char *Version(void) { return PluginVersion; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
#if APIVERSNUM >= 10131
  virtual bool Initialize(void);
#else
  virtual bool Start(void);
#endif
  virtual const char *MainMenuEntry(void);
  virtual cOsdMenu *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
#if APIVERSNUM >= 10330
  virtual bool Service(const char *Id, void *Data);
#if APIVERSNUM >= 10331
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
#endif
#endif
  };

cPluginMPlayer::cPluginMPlayer(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  status=0;
}

cPluginMPlayer::~cPluginMPlayer()
{
  delete status;
}

const char *cPluginMPlayer::CommandLineHelp(void)
{
  static char *help_str=0;
  
  free(help_str);    //                                     for easier orientation, this is column 80|
  asprintf(&help_str,"  -m CMD,   --mount=CMD    use CMD to mount/unmount/eject mp3 sources\n"
                     "                           (default: %s)\n"
                     "  -M CMD,   --mplayer=CMD  use CMD when calling MPlayer\n"
                     "                           (default: %s)\n"
                     "  -S SUB,   --sources=SUB  search sources config in SUB subdirectory\n"
                     "                           (default: %s)\n"
                     "  -R DIR,   --resume=DIR   store global resume file in DIR\n"
                     "                           (default: %s)\n",
                     mountscript,
                     MPlayerCmd,
                     sourcesSub ? sourcesSub:"none",
                     globalResumeDir ? globalResumeDir:"video dir"
                     );
  return help_str;
}

bool cPluginMPlayer::ProcessArgs(int argc, char *argv[])
{
  static struct option long_options[] = {
      { "mount",    required_argument, NULL, 'm' },
      { "mplayer",  required_argument, NULL, 'M' },
      { "sources",  required_argument, NULL, 'S' },
      { "resume",   required_argument, NULL, 'R' },
      { NULL }
    };

  int c, option_index = 0;
  while((c=getopt_long(argc,argv,"m:M:S:R:",long_options,&option_index))!=-1) {
    switch (c) {
      case 'm': mountscript=optarg; break;
      case 'M': MPlayerCmd=optarg; break;
      case 'S': sourcesSub=optarg; break;
      case 'R': globalResumeDir=optarg; break;
      default:  return false;
      }
    }
  return true;
}

#if APIVERSNUM >= 10131
bool cPluginMPlayer::Initialize(void)
#else
bool cPluginMPlayer::Start(void)
#endif
{
  if(!CheckVDRVersion(1,1,16,"mplayer")) return false;
  plugin_name="mplayer";
#if APIVERSNUM < 10507
  i18n_name="mplayer";
#else
  i18n_name="vdr-mplayer";
#endif
  MPlaySources.Load(AddDirectory(ConfigDirectory(sourcesSub),"mplayersources.conf"));
  if(MPlaySources.Count()<1) {
    esyslog("ERROR: you must have defined at least one source in mplayersources.conf");
    fprintf(stderr,"No source(s) defined in mplayersources.conf\n");
    return false;
    }
#if APIVERSNUM < 10507
  RegisterI18n(Phrases);
#endif
  if(!(status=new cMPlayerStatus)) return false;
  return true;
}

const char *cPluginMPlayer::MainMenuEntry(void)
{
  return MPlayerSetup.HideMainMenu ? 0 : tr(MAINMENUENTRY);
}

cOsdMenu *cPluginMPlayer::MainMenuAction(void)
{
  return new cMenuMPlayBrowse;
}

cMenuSetupPage *cPluginMPlayer::SetupMenu(void)
{
  return new cMenuSetupMPlayer;
}

bool cPluginMPlayer::SetupParse(const char *Name, const char *Value)
{
  if(      !strcasecmp(Name, "ControlMode"))  MPlayerSetup.SlaveMode    = atoi(Value);
  else if (!strcasecmp(Name, "HideMainMenu")) MPlayerSetup.HideMainMenu = atoi(Value);
  else if (!strcasecmp(Name, "ResumeMode"))   MPlayerSetup.ResumeMode   = atoi(Value);
  else if (!strcasecmp(Name, "OsdPos"))       MPlayerSetup.OsdPos       = atoi(Value);
  else if (!strncasecmp(Name,"KeyCmd", 6) && strlen(Name)==7 && isdigit(Name[6]))
    strn0cpy(MPlayerSetup.KeyCmd[Name[6]-'0'],Value,sizeof(MPlayerSetup.KeyCmd[0]));
  else return false;
  return true;
}

#if APIVERSNUM >= 10330

bool cPluginMPlayer::ExternalPlay(const char *path, bool test)
{
  char real[PATH_MAX+1];
  if(realpath(path,real)) {
    cFileSource *src=MPlaySources.FindSource(real);
    if(src) {
      cFileObj *item=new cFileObj(src,0,0,otFile);
      if(item) {
        item->SplitAndSet(real);
        if(item->GuessType()) {
          if(item->Exists()) {
            if(!test) {
              cMPlayerControl::SetFile(item,true);
              cControl::Launch(new cMPlayerControl);
              cControl::Attach();
              }
            delete item;
            return true;
            }
          else dsyslog("MPlayer service: cannot play '%s'",path);
          }
        else dsyslog("MPlayer service: GuessType() failed for '%s'",path);
        delete item;
        }
      }
    else dsyslog("MPlayer service: cannot find source for '%s', real '%s'",path,real);
    }
  else if(errno!=ENOENT && errno!=ENOTDIR)
    esyslog("ERROR: realpath: %s: %s",path,strerror(errno));
  return false;
}

bool cPluginMPlayer::Service(const char *Id, void *Data)
{
  if(!strcasecmp(Id,"MPlayer-Play-v1")) {
    if(Data) {
      struct MPlayerServiceData *msd=(struct MPlayerServiceData *)Data;
      msd->result=ExternalPlay(msd->data.filename,false);
      }
    return true;
    }
  else if(!strcasecmp(Id,"MPlayer-Test-v1")) {
    if(Data) {
      struct MPlayerServiceData *msd=(struct MPlayerServiceData *)Data;
      msd->result=ExternalPlay(msd->data.filename,true);
      }
    return true;
    }
  return false;
}

#if APIVERSNUM >= 10331

const char **cPluginMPlayer::SVDRPHelpPages(void)
{
  static const char *HelpPages[] = {
    "PLAY <filename>\n"
    "    Triggers playback of file 'filename'.",
    "TEST <filename>\n"
    "    Tests is playback of file 'filename' is possible.",
    NULL
    };
  return HelpPages;
}

cString cPluginMPlayer::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  if(!strcasecmp(Command,"PLAY")) {
    if(*Option) {
      if(ExternalPlay(Option,false)) return "Playback triggered";
      else { ReplyCode=550; return "Playback failed"; }
      }
    else { ReplyCode=501; return "Missing filename"; }
    }
  else if(!strcasecmp(Command,"TEST")) {
    if(*Option) {
      if(ExternalPlay(Option,true)) return "Playback possible";
      else { ReplyCode=550; return "Playback not possible"; }
      }
    else { ReplyCode=501; return "Missing filename"; }
    }
  return NULL;
}

#endif // 1.3.31
#endif // 1.3.30

VDRPLUGINCREATOR(cPluginMPlayer); // Don't touch this!
