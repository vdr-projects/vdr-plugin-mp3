/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2010 Stefan Huelswitt <s.huelswitt@gmx.de>
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

#ifndef ___DECODER_OGG_STREAM_H
#define ___DECODER_OGG_STREAM_H

#define DEC_OGGS     DEC_ID('O','G','G','S')
#define DEC_OGGS_STR "OGGS"

#ifdef HAVE_VORBISFILE

#include "decoder-ogg.h"

class cNetStream;

// ----------------------------------------------------------------

class cNetOggFile : public cOggFile {
friend class cNetOggInfo;
private:
  cNetStream *nstr;
public:
  cNetOggFile(const char *Filename);
  virtual bool Open(bool log=true);
  };

// ----------------------------------------------------------------

class cNetOggInfo : public cOggInfo {
private:
  cNetOggFile *nfile;
  cNetStream *nstr;
  void IcyInfo(void);
public:
  cNetOggInfo(cNetOggFile *File);
  virtual bool DoScan(bool KeepOpen=false);
  virtual void InfoHook(void);
  };

// ----------------------------------------------------------------

class cOggStreamDecoder : public cOggDecoder {
private:
  cNetOggFile *nfile;
public:
  cOggStreamDecoder(const char *Filename);
  virtual bool IsStream(void) { return true; }
  };

#endif //HAVE_VORBISFILE
#endif //___DECODER_OGG_STREAM_H
