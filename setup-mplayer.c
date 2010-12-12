/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2010 Stefan Huelswitt <s.huelswitt@gmx.de>
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

#include <string.h>

#include "common.h"
#include "setup-mplayer.h"

cMPlayerSetup MPlayerSetup;

// --- cMPlayerSetup -----------------------------------------------------------

cMPlayerSetup::cMPlayerSetup(void)
{
  SlaveMode = 1;
  ResumeMode = 2;
  HideMainMenu = 0;
  PrevNextKeyMode = 0;
  memset(KeyCmd,0,sizeof(KeyCmd));
  strcpy(KeyCmd[1],"audio_delay +0.1");
  strcpy(KeyCmd[7],"audio_delay -0.1");
  strcpy(KeyCmd[4],"switch_audio");
}
