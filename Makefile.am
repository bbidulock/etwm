ACLOCAL_AMFLAGS	 = -I m4

EXTRA_DIST = INSTALL COPYING AUTHORS AUTHORS.in NEWS NEWS.in README README.in THANKS THANKS.in TODO TODO.in ChangeLog

ChangeLog:
	( \
	  echo -e "# created with git log --stat=76 -M -C|fmt -sct -w80\n" ; \
	  git log --stat=76 -M -C|fmt -sct -w80 ; \
	)>$@

AUTHORS.in:
	( \
	  echo "\@PACKAGE\@ -- authors file.  \@DATE\@" ; \
	  echo "" ; \
	  git log|grep '^Author:'|awk '{if(!authors[$$0]){print$$0;authors[$$0]=1;}}'|tac \
	)>$@

README: README.in
	sed -r -e 's:\@PACKAGE\@:$(PACKAGE):g;s:\@VERSION\@:$(VERSION):g;s:\@DATE\@:$(DATE):g' $< >$@

NEWS: NEWS.in
	sed -r -e 's:\@PACKAGE\@:$(PACKAGE):g;s:\@VERSION\@:$(VERSION):g;s:\@DATE\@:$(DATE):g' $< >$@

AUTHORS: AUTHORS.in
	sed -r -e 's:\@PACKAGE\@:$(PACKAGE):g;s:\@VERSION\@:$(VERSION):g;s:\@DATE\@:$(DATE):g' $< >$@

THANKS: THANKS.in
	sed -r -e 's:\@PACKAGE\@:$(PACKAGE):g;s:\@VERSION\@:$(VERSION):g;s:\@DATE\@:$(DATE):g' $< >$@

TODO: TODO.in
	sed -r -e 's:\@PACKAGE\@:$(PACKAGE):g;s:\@VERSION\@:$(VERSION):g;s:\@DATE\@:$(DATE):g' $< >$@

dist_noinst_SCRIPTS = autogen.sh configure.sh

EXTRA_DIST += siconify.bm

AM_YFLAGS = -d

bin_PROGRAMS = etwm
lib_LIBRARIES = libetwm.a

AM_INSTALLCHECK_STD_OPTIONS_EXEMPT = etwm

etwmdir = $(sysconfdir)/X11/etwm
pixmapdir = $(etwmdir)/pixmaps

dist_etwm_DATA = system.etwmrc

AM_CPPFLAGS = -include config.h \
	'-DSYSTEM_INIT_FILE="$(etwmdir)/system.etwmrc"' \
	'-DPIXMAP_DIRECTORY="$(pixmapdir)"' \
	'-DYY_NO_INPUT'

AM_CFLAGS = $(ETWM_CFLAGS) $(xpm_CFLAGS) $(xrandr_CFLAGS) $(xinerama_CFLAGS)

libetwm_a_SOURCES = \
	twm.h \
	etwm.h \
	libetwm.c

etwm_LDADD = $(ETWM_LIBS) $(xpm_LIBS) $(xrandr_LIBS) $(xinerama_LIBS) libetwm.a

EXTRA_etwm_SOURCES = \
	ewmh.c \
	ewmh.h \
	ewmh_etwm.c \
	wmh.c \
	wmh.h \
	wmh_etwm.c \
	mwmh.c \
	mwmh.h \
	mwmh_etwm.c \
	gnome.c \
	gnome.h \
	gnomewindefs.h \
	sound.c \
	sound.h \
	lnm.c \
	lnm.h \
	vms_cmd_services.c \
	vms_cmd_services.h

etwm_SOURCES = \
	add_window.c \
	add_window.h \
	clicktofocus.c \
	clicktofocus.h \
	etwm.c \
	etwm.h \
	cursor.c \
	cursor.h \
	deftwmrc.c \
	events.c \
	events.h \
	gc.c \
	gc.h \
	gram.y \
	iconmgr.c \
	iconmgr.h \
	icons.c \
	icons.h \
	lex.l \
	list.c \
	list.h \
	menus.c \
	menus.h \
	parse.c \
	parse.h \
	resize.c \
	resize.h \
	screen.h \
	session.c \
	session.h \
	twm.h \
	types.h \
	util.c \
	util.h \
	version.c \
	version.h \
	vscreen.c \
	vscreen.h \
	windowbox.c \
	windowbox.h \
	workmgr.c \
	workmgr.h

if GNOME
etwm_SOURCES += \
	gnome.c \
	gnome.h \
	gnomewindefs.h
endif

if SOUND
etwm_SOURCES += \
	sound.c \
	sound.h
endif

if EWMH
etwm_SOURCES += \
	ewmh.c \
	ewmh.h \
	ewmh_etwm.c
endif

if WMH
etwm_SOURCES += \
	wmh.c \
	wmh.h \
	wmh_etwm.c
endif

if MWMH
etwm_SOURCES += \
	mwmh.c \
	mwmh.h \
	mwmh_etwm.c
endif

dist_pixmap_DATA = \
	xpm/3dcircle.xpm \
	xpm/3ddimple.xpm \
	xpm/3ddot.xpm \
	xpm/3D_Expand15.xpm \
	xpm/3dfeet.xpm \
	xpm/3D_Iconify15.xpm \
	xpm/3dleopard.xpm \
	xpm/3D_Lightning15.xpm \
	xpm/3D_Menu15.xpm \
	xpm/3dpie.xpm \
	xpm/3dpyramid.xpm \
	xpm/3D_Resize15.xpm \
	xpm/3dslant.xpm \
	xpm/3D_Zoom15.xpm \
	xpm/arthur.xpm \
	xpm/audio_editor.xpm \
	xpm/background1.xpm \
	xpm/background2.xpm \
	xpm/background3.xpm \
	xpm/background4.xpm \
	xpm/background5.xpm \
	xpm/background6.xpm \
	xpm/background7.xpm \
	xpm/background8.xpm \
	xpm/background9.xpm \
	xpm/ball10.xpm \
	xpm/ball11.xpm \
	xpm/ball12.xpm \
	xpm/ball1.xpm \
	xpm/ball2.xpm \
	xpm/ball3.xpm \
	xpm/ball4.xpm \
	xpm/ball5.xpm \
	xpm/ball6.xpm \
	xpm/ball7.xpm \
	xpm/ball8.xpm \
	xpm/ball9.xpm \
	xpm/cdrom1.xpm \
	xpm/claude.xpm \
	xpm/clipboard.xpm \
	xpm/datebook.xpm \
	xpm/emacs.xpm \
	xpm/ghostview.xpm \
	xpm/hpterm.xpm \
	xpm/IslandD.xpm \
	xpm/IslandW.xpm \
	xpm/LRom1.xpm \
	xpm/LRom.xpm \
	xpm/mail0.xpm \
	xpm/mail1.xpm \
	xpm/nothing.xpm \
	xpm/nt1.xpm \
	xpm/nt2.xpm \
	xpm/pixmap.xpm \
	xpm/postit.xpm \
	xpm/skull.xpm \
	xpm/spider.xpm \
	xpm/supman1.xbm \
	xpm/supman2.xbm \
	xpm/supman3.xbm \
	xpm/supman4.xbm \
	xpm/supman5.xbm \
	xpm/supman6.xbm \
	xpm/supman7.xbm \
	xpm/supman8.xbm \
	xpm/supman9.xbm \
	xpm/term.xpm \
	xpm/unknown1.xpm \
	xpm/unknown.xpm \
	xpm/unread.xpm \
	xpm/welcome.xpm \
	xpm/welcome.xwd \
	xpm/xarchie.xpm \
	xpm/xcalc2.xpm \
	xpm/xcalc.xpm \
	xpm/xedit.xpm \
	xpm/xftp.xpm \
	xpm/xgopher.xpm \
	xpm/xgrab.xpm \
	xpm/xhpcalc.xpm \
	xpm/xirc.xpm \
	xpm/xmail.xpm \
	xpm/xman.xpm \
	xpm/xmosaic.xpm \
	xpm/xnomail.xpm \
	xpm/xrn-compose.xpm \
	xpm/xrn.goodnews.xpm \
	xpm/xrn.nonews.xpm \
	xpm/xrn.xpm \
	xpm/xterm.xpm

BUILT_SOURCES = gram.h deftwmrc.c

deftwmrc.c: system.etwmrc
	$(AM_V_at)rm -f $@ ; \
	echo '/* ' >>$@ ; \
	echo ' * This file is generated automatically from the default' >>$@ ; \
	echo ' * etwm bindings file system.etwmrc by the etwm Makefile.' >>$@ ; \
	echo ' */' >>$@ ; \
	echo '' >>$@ ; \
	echo 'char *defTwmrc[] = {' >>$@ ; \
	sed -e '/^#/d' -e 's/"/\\"/g' -e 's/^/    "/' -e 's/$$/",/' ${srcdir}/system.etwmrc >>$@ ; \
	echo '    (char *) 0 };' >>$@

dist_man1_MANS = etwm.man
