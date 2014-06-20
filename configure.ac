#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([etwm],[3.8.3],[http://github.com/bbidulock/issues],[etwm])
AC_CONFIG_SRCDIR([iconmgr.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([scripts])
AC_CONFIG_MACRO_DIR([m4])
AC_USE_SYSTEM_EXTENSIONS

# Initialize Automake
AM_INIT_AUTOMAKE([gnits 1.14 dist-xz no-dist-gzip std-options -Wall color-tests silent-rules tar-pax])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_PROG_AR
AC_PROG_MKDIR_P
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([fcntl.h inttypes.h libintl.h limits.h malloc.h netdb.h stddef.h stdlib.h string.h sys/param.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRNLEN
AC_CHECK_FUNCS([dup2 gethostbyname gethostname gettimeofday isascii memset mkstemp putenv regcomp select setlocale strchr strdup strncasecmp strrchr strstr])

AC_ARG_ENABLE([gnome],AC_HELP_STRING([--enable-gnome],[enable WinWM/WMH (GNOME) support]))
if test "x$enable_gnome" = "xyes"; then
    AC_DEFINE_UNQUOTED([GNOME],[],[Define to enable WinWM/WMH (GNOME) support.])
fi
AM_CONDITIONAL([GNOME],[test "x$enable_gnome" = 'xyes'])

AC_ARG_ENABLE([xinerama],AC_HELP_STRING([--disable-xinerama],[disable Xinerama support]))
if test "x$enable_xinerama" != "xno"; then
    PKG_CHECK_EXISTS([xinerama],[PKG_CHECK_MODULES([xinerama],[xinerama])],[enable_xinerama=no])
fi
if test "x$enable_xinerama" != "xno"; then
    AC_DEFINE_UNQUOTED([USE_XINERAMA],[],[Define to enable Xinerama support.])
fi

AC_ARG_ENABLE([xrandr],AC_HELP_STRING([--disable-xrandr],[disable XRANDR support]))
if test "x$enable_xrandr" != "xno"; then
    PKG_CHECK_EXISTS([xrandr],[PKG_CHECK_MODULES([xrandr],[xrandr])],[enable_xrandr=no])
fi
if test "x$enable_xrandr" != "xno"; then
    AC_DEFINE_UNQUOTED([USE_XRANDR],[],[Define to enable XRANDR support.])
fi

AC_ARG_ENABLE([xpm],AC_HELP_STRING([--disable-xpm],[disable libXpm support]))
if test "x$enable_xpm" != "xno"; then
    PKG_CHECK_EXISTS([xpm],[PKG_CHECK_MODULES([xpm],[xpm])],[enable_xpm=no])
fi
if test "x$enable_xpm" != "xno"; then
    AC_DEFINE_UNQUOTED([XPM],[],[Define to enable libXpm support.])
fi

AC_ARG_ENABLE([jpeg],AC_HELP_STRING([--disable-jpeg],[disable libjpeg support]))
if test "x$enable_jpeg" != "xno"; then
    AC_DEFINE_UNQUOTED([JPEG],[],[Define to enable libjpeg support.])
fi

AC_ARG_ENABLE([imconv],AC_HELP_STRING([--enable-imconv],[enable imconv support]))
if test "x$enable_imconv" = "xyes" ; then
    AC_DEFINE_UNQUOTED([IMCONV],[],[Define to enable imconv support.])
fi

AC_ARG_ENABLE([m4],AC_HELP_STRING([--disable-m4],[disable M4 support]))
if test "x$enable_m4" != "xno" ; then
    AC_DEFINE_UNQUOTED([USEM4],[],[Define to enable M4 support.])
fi

AC_ARG_ENABLE([sound],AC_HELP_STRING([--enable-sound],[enable sound support]))
if test "x$enable_sound" = "xyes" ; then
    AC_DEFINE_UNQUOTED([SOUNDS],[],[Define for sound support.])
fi
AM_CONDITIONAL([SOUND],[test "x$enable_sound" = "xyes"])

AC_ARG_ENABLE([ewmh],AC_HELP_STRING([--disable-ewmh],[disable NetWM/EWMH support]))
if test "x$enable_ewmh" != "xno" ; then
    AC_DEFINE_UNQUOTED([EWMH],[],[Define to enable NetWM/EWMH support.])
fi
AM_CONDITIONAL([EWMH],[test "x$enable_ewmh" != "xno"])

AC_ARG_ENABLE([wmh],AC_HELP_STRING([--disable-wmh],[disable WinWM/WMH support]))
if test "x$enable_wmh" != "xno" ; then
    AC_DEFINE_UNQUOTED([WMH],[],[Define to enable WinWM/WMH support.])
fi
AM_CONDITIONAL([WMH],[test "x$enable_wmh" != "xno"])

AC_ARG_ENABLE([mwmh],AC_HELP_STRING([--disable-mwmh],[disable Motif/MWMH support]))
if test "x$enable_mwmh" != "xno" ; then
    AC_DEFINE_UNQUOTED([MWMH],[],[Define to enable Motif/MWMH support.])
fi
AM_CONDITIONAL([MWMH],[test "x$enable_mwmh" != "xno"])

AC_ARG_ENABLE([gnu-regex],AC_HELP_STRING([--enable-gnu-regex],[enable use of GNU regex]),[
   if test "x$enableval" = "xyes" ; then
       AC_DEFINE_UNQUOTED([USE_GNU_REGEX],[],[Define to use GNU regular expressions.])
   fi])

AC_ARG_ENABLE([signals],AC_HELP_STRING([--disable-signals],[disable use of signals]))
if test "x$enable_signals" != "xno" ; then
    AC_DEFINE_UNQUOTED([USE_SIGNALS],[],[Define to support response to signals.])
fi

dnl AC_ARG_ENABLE([xinerama],AC_HELP_STRING([--enable-xinerama],[enable XINERAMA support]))
dnl AC_ARG_ENABLE([xft],AC_HELP_STRING([--enable-xft],[enable libXft support]))

AC_ARG_ENABLE([debug],AC_HELP_STRING([--enable-debug],[enable debugging]),[
    if test "x$enableval" = "xyes"; then
        AC_DEFINE_UNQUOTED([DEBUG],[],[Define to enable debugging.])
    fi])
AC_ARG_ENABLE([debug-events],AC_HELP_STRING([--enable-debug-events],[enable events debugging]),[
    if test "x$enableval" = "xyes"; then
        AC_DEFINE_UNQUOTED([DEBUG_EVENTS],[],[Define to enable events debugging.])
    fi])
AC_ARG_ENABLE([debug-iconmgr],AC_HELP_STRING([--enable-debug-iconmgr],[enable icon manager debugging]),[
    if test "x$enableval" = "xyes"; then
        AC_DEFINE_UNQUOTED([DEBUG_ICONMGR],[],[Define to enable icon manager debugging.])
    fi])
AC_ARG_ENABLE([debug-menus],AC_HELP_STRING([--enable-debug-menus],[enable menu debugging]),[
    if test "x$enableval" = "xyes"; then
        AC_DEFINE_UNQUOTED([DEBUG_MENU],[],[Define to enable menu debugging.])
    fi])
AC_ARG_ENABLE([debug-ewmh],AC_HELP_STRING([--enable-debug-ewmh],[enable NetWM/EWMH debugging]),[
    if test "x$enableval" = "xyes"; then
        AC_DEFINE_UNQUOTED([DEBUG_EWMH],[],[Define to enable NetWM/EWMH debugging.])
    fi])
AC_ARG_ENABLE([debug-wmh],AC_HELP_STRING([--enable-debug-wmh],[enable WinWM/WMH debugging]),[
    if test "x$enableval" = "xyes"; then
        AC_DEFINE_UNQUOTED([DEBUG_WMH],[],[Define to enable WinWM/WMH debugging.])
    fi])
AC_ARG_ENABLE([debug-mwmh],AC_HELP_STRING([--enable-debug-mwmh],[enable Motif/MWMH debugging]),[
    if test "x$enableval" = "xyes"; then
	AC_DEFINE_UNQUOTED([DEBUG_MWMH],[],[Define to enable Motif/MWMH debugging.])
    fi])


# Checks for pkg-config packages
PKG_CHECK_MODULES([ETWM], [x11 xext xt xmu ice sm xproto])

ETWM_LIBS="$ETWM_LIBS -ljpeg"


AC_CONFIG_FILES([Makefile])

AC_OUTPUT
