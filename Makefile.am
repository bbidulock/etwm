AM_YFLAGS = -d

bin_PROGRAMS = ctwm
lib_LIBRARIES = libctwm.a

ctwmdir = $(sysconfdir)/X11/ctwm
pixmapdir = $(ctwmdir)/pixmaps

dist_ctwm_DATA = system.ctwmrc

AM_CPPFLAGS = -include config.h \
	'-DSYSTEM_INIT_FILE="$(ctwmdir)/system.ctwmrc"' \
	'-DPIXMAP_DIRECTORY="$(pixmapdir)"'

AM_CFLAGS = $(CTWM_CFLAGS)

libctwm_a_SOURCES = \
	twm.h \
	ctwm.h \
	libctwm.c

ctwm_LDADD = $(CTWM_LIBS) libctwm.a

EXTRA_ctwm_SOURCES = \
	gnome.c \
	gnome.h \
	gnomewindefs.h \
	sound.c \
	sound.h \
	lnm.c \
	lnm.h \
	vms_cmd_services.c \
	vms_cmd_services.h

ctwm_SOURCES = \
	add_window.c \
	add_window.h \
	clicktofocus.c \
	clicktofocus.h \
	ctwm.c \
	ctwm.h \
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
ctwm_SOURCES += \
	gnome.c \
	gnome.h \
	gnomewindefs.h
endif

if SOUND
ctwm_SOURCES += \
	sound.c \
	sound.h
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

deftwmrc.c: system.ctwmrc
	$(AM_V_at)rm -f $@ ; \
	echo '/* ' >>$@ ; \
	echo ' * This file is generated automatically from the default' >>$@ ; \
	echo ' * ctwm bindings file system.ctwmrc by the ctwm Makefile.' >>$@ ; \
	echo ' */' >>$@ ; \
	echo '' >>$@ ; \
	echo 'char *defTwmrc[] = {' >>$@ ; \
	sed -e '/^#/d' -e 's/"/\\"/g' -e 's/^/    "/' -e 's/$$/",/' ${srcdir}/system.ctwmrc >>$@ ; \
	echo '    (char *) 0 };' >>$@

man1_MANS = ctwm.man
