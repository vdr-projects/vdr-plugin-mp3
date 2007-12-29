#
# MP3/MPlayer plugin to VDR
#
# (C) 2001-2007 Stefan Huelswitt <s.huelswitt@gmx.de>
#
# This code is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This code is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# Or, point your browser to http://www.gnu.org/copyleft/gpl.html

# You can change the compile options here or create a Make.config
# in the VDR directory an set them there.

### uncomment one of these lines, if you don't want one of the plugins
#WITHOUT_MP3=1
#WITHOUT_MPLAYER=1

### uncomment the following line, if you don't have libsndfile installed
#WITHOUT_LIBSNDFILE=1

### uncomment the following line, if you don't have libvorbisfile installed
#WITHOUT_LIBVORBISFILE=1

### uncomment the following line, if you want OSS sound output
#WITH_OSS_OUTPUT=1

### uncomment the following line, if you want to include debug symbols
#DBG=1

### The C++ compiler and options:
CXX      ?= g++
CXXFLAGS ?= -O2 -fPIC -Wall -Woverloaded-virtual

###############################################
###############################################
#
# no user configurable options below this point
#
###############################################
###############################################

### The directory environment:

VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN  = mp3
PLUGIN2 = mplayer

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config
-include Make.config

### The version number of this plugin:

RELEASE := $(shell grep 'define PLUGIN_RELEASE' version.h | awk '{ print $$3 }' | sed -e 's/[";]//g')
VERSION := $(shell if test -d .hg; then echo -n '$(RELEASE)-'; (hg identify 2>/dev/null || echo -n Unknown) | sed -e 's/ .*//'; else echo -n '$(RELEASE)'; fi)

### The version number of VDR (taken from VDR's "config.h"):

VDRVERSION := $(shell sed -ne '/define VDRVERSION/ s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)
APIVERSION := $(shell sed -ne '/define APIVERSION/ s/^.*"\(.*\)".*$$/\1/p' $(VDRDIR)/config.h)
ifeq ($(strip $(APIVERSION)),)
   APIVERSION = $(VDRVERSION)
endif
VDRVERSNUM := $(shell sed -ne '/define VDRVERSNUM/ s/^.[a-zA-Z ]*\([0-9]*\) .*$$/\1/p' $(VDRDIR)/config.h)
APIVERSNUM := $(shell sed -ne '/define APIVERSNUM/ s/^.[a-zA-Z ]*\([0-9]*\) .*$$/\1/p' $(VDRDIR)/config.h)
ifeq ($(strip $(APIVERSNUM)),)
   APIVERSNUM = $(VDRVERSNUM)
endif

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include
DEFINES  += -D_GNU_SOURCE -DAPIVERSNUM=$(APIVERSNUM)

### The object files (add further files here):

ifndef WITHOUT_MP3
  ALL += libvdr-$(PLUGIN).so
  ifneq ($(shell grep -l 'Phrases' $(VDRDIR)/i18n.c),$(VDRDIR)/i18n.c)
    ALL += i18n-$(PLUGIN)
  endif
endif
ifndef WITHOUT_MPLAYER
  ALL += libvdr-$(PLUGIN2).so
  ifneq ($(shell grep -l 'Phrases' $(VDRDIR)/i18n.c),$(VDRDIR)/i18n.c)
    ALL += i18n-$(PLUGIN2)
  endif
endif

COM_OBJS = i18n.o data.o menu.o version.o

OBJS     = $(PLUGIN).o $(COM_OBJS)\
            data-mp3.o setup-mp3.o player-mp3.o stream.o network.o\
            decoder.o decoder-mp3.o decoder-mp3-stream.o decoder-snd.o \
            decoder-ogg.o
LIBS     = -lmad -lid3tag

ifndef WITHOUT_LIBSNDFILE
  LIBS    += -lsndfile
  DEFINES += -DHAVE_SNDFILE
endif
ifndef WITHOUT_LIBVORBISFILE
  LIBS    += -lvorbisfile -lvorbis
  DEFINES += -DHAVE_VORBISFILE
endif
ifdef WITH_OSS_OUTPUT
  DEFINES += -DWITH_OSS
endif
ifdef BROKEN_PCM
  DEFINES += -DBROKEN_PCM
endif

OBJS2    = $(PLUGIN2).o $(COM_OBJS)\
            setup-mplayer.o player-mplayer.o
LIBS2    = 

ifeq ($(shell test -f $(VDRDIR)/fontsym.h ; echo $$?),0)
  DEFINES += -DHAVE_BEAUTYPATCH
endif  

ifdef DBG
  CXXFLAGS += -g
endif

### Internationalization (I18N):

PODIR     = po
I18Npot   = $(PODIR)/mp3-mplayer.pot
I18Npots  = $(notdir $(foreach file, $(wildcard $(PODIR)/*.po), $(basename $(file))))
I18Nmsgs  = $(addprefix $(LOCALEDIR)/,$(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo,$(I18Npots)))
I18Nmsgs2 = $(addprefix $(LOCALEDIR)/,$(addsuffix /LC_MESSAGES/vdr-$(PLUGIN2).mo,$(I18Npots)))
LOCALEDIR = $(VDRDIR)/locale

# Dependencies:

MAKEDEP = g++ -MM -MG
DEPFILE = .dependencies
DEPFILES = $(subst version.c,,$(OBJS:%.o=%.c) $(OBJS2:%.o=%.c))
$(DEPFILE): Makefile $(DEPFILES) $(wildcard *.h)
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(DEPFILES) > $@

-include $(DEPFILE)

### Targets:

all: $(ALL)
.PHONY: i18n-$(PLUGIN) i18n-$(PLUGIN2)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@
	@cp $@ $(LIBDIR)/$@.$(APIVERSION)

libvdr-$(PLUGIN2).so: $(OBJS2)
	$(CXX) $(CXXFLAGS) -shared $(OBJS2) $(LIBS2) -o $@
	@cp $@ $(LIBDIR)/$@.$(APIVERSION)

$(I18Npot): $(shell grep -rl '\(tr\|trNOOP\)(\".*\")' *.c )
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --msgid-bugs-address='<s.huelswitt@gmx.de>' -o $@ $^

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q $@ $<
	@touch $@

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Nmsgs): $(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	@mkdir -p $(dir $@)
	cp $< $@

i18n-$(PLUGIN): $(I18Nmsgs)

$(I18Nmsgs2): $(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN2).mo: $(PODIR)/%.mo
	@mkdir -p $(dir $@)
	cp $< $@

i18n-$(PLUGIN2): $(I18Nmsgs2)

version.c: FORCE
	@echo >$@.new "/* this file will be overwritten without warning */"; \
	 echo >>$@.new 'const char *PluginVersion =' '"'$(VERSION)'";'; \
	 diff $@.new $@ >$@.diff 2>&1; \
	 if test -s $@.diff; then mv -f $@.new $@; fi; \
	 rm -f $@.new $@.diff;

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tar.gz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tar.gz

clean:
	@-rm -f $(OBJS) $(OBJS2) $(DEPFILE) libvdr-*.so $(PACKAGE).tar.gz core* *~
	@-rm -f version.c
	@-rm -f $(PODIR)/*.mo

FORCE: