#!/bin/bash

# need --enable-maintainer-mode to be able to run in place
#      must be disabled to build an installable package

# *FLAGS are what Arch Linux makepkg uses with the exception
#      that -Wall -Werror is added

case "`uname -m`" in
	i686)
		CPPFLAGS="-D_FORTIFY_SOURCE"
		CFLAGS="-march=i686 -mtune=generic -O2 -pipe -fstack-protector-strong --param=ssp-buffer-size=4"
		CXXFLAGS="-march=i686 -mtune=generic -O2 -pipe -fstack-protector-strong --param=ssp-buffer-size=4"
		LDFLAGS="-Wl,-O1,--sort-common,--as-needed,-z,relro"
		DEBUG_CFLAGS="-g -fvar-tracking-assignments"
		DEBUG_CXXFLAGS="-g -fvar-tracking-assignments"
	;;
	x86_64)
		CPPFLAGS="-D_FORTIFY_SOURCE=2"
		CFLAGS="-march=x86-64 -mtune=generic -O2 -pipe -fstack-protector-strong --param=ssp-buffer-size=4"
		CXXFLAGS="-march=x86-64 -mtune=generic -O2 -pipe -fstack-protector-strong --param=ssp-buffer-size=4"
		LDFLAGS="-Wl,-O1,--sort-common,--as-needed,-z,relro"
		DEBUG_CFLAGS="-g -fvar-tracking-assignments"
		DEBUG_CXXFLAGS="-g -fvar-tracking-assignments"
	;;
esac

./configure \
	--enable-maintainer-mode \
	--enable-dependency-tracking \
	--prefix=/usr \
	--sysconfdir=/etc \
	CPPFLAGS="$CPPFLAGS" \
	CFLAGS="-Wall -Werror $CFLAGS" \
	CXXFLAGS="-Wall -Werror $CXXFLAGS" \
	LDFLAGS="$LDFLAGS" \
	DEBUG_CFLAGS="$DEBUG_CFLAGS" \
	DEBUG_CXXFLAGS="$DEBUG_CXXFLAGS"
