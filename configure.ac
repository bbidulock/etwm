#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ctwm], [3.8.3], [http://github.com/bbidulock], [ctwm])
AC_CONFIG_SRCDIR([iconmgr.h])
AC_CONFIG_HEADERS([config.h])
AC_USE_SYSTEM_EXTENSIONS

# Initialize Automake
AM_INIT_AUTOMAKE([foreign dist-bzip2])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_RANLIB

enable_gnome=yes
enable_sound=no

AM_CONDITIONAL([GNOME],[test $enable_gnome = 'yes'])
AM_CONDITIONAL([SOUND],[test $enable_sound = 'yes'])

AC_DEFINE_UNQUOTED([GNOME],[],[Define for Gnome1 support.])
AC_DEFINE_UNQUOTED([XPM],[],[Define for libXpm support.])
AC_DEFINE_UNQUOTED([JPEG],[],[Define for libjpeg support.])
dnl AC_DEFINE_UNQUOTED([IMCONV],[],[Define for imconv support.])
AC_DEFINE_UNQUOTED([USEM4],[],[Define for m4 support.])
dnl AC_DEFINE_UNQUOTED([SOUNDS],[],[Define for sound support.])

# Checks for libraries.

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([fcntl.h limits.h netdb.h stdlib.h string.h sys/param.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([dup2 gethostbyname gettimeofday isascii memset putenv regcomp select setlocale strchr strdup strncasecmp strrchr strstr])

# Checks for pkg-config packages
PKG_CHECK_MODULES([CTWM], [x11 xext xt xmu ice sm xproto xpm])

CTWM_LIBS="$CTWM_LIBS -ljpeg"


AC_CONFIG_FILES([Makefile])

AC_OUTPUT
