AC_INIT(lb_panel, [0.1.0], [])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([app/panel/TMainPageView.cpp])

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([no-define])
LT_INIT([disable-static])

AC_PROG_CC
AC_PROG_CXX

AC_ARG_WITH(lite_beapi_config,
           [AC_HELP_STRING([--with-lite-beapi-config=],
                           [specify path of lite-beapi-config])],
           [if test "x$with_lite_beapi_config" = x; then
		LITE_BEAPI_CONFIG=no
	    else
		LITE_BEAPI_CONFIG=$with_lite_beapi_config
	    fi],
	   [LITE_BEAPI_CONFIG=no])

AC_ARG_WITH(lite_beapi_cflags,
           [AC_HELP_STRING([--with-lite-beapi-cflags=],
                           [specify C compiler flags for Lite BeAPI])],
           [if test "x$with_lite_beapi_cflags" = x; then
		spec_lite_beapi_cflags=no
	    else
		spec_lite_beapi_cflags=yes
	    fi],
	   [spec_lite_beapi_cflags=no])

AC_ARG_WITH(lite_beapi_libs,
           [AC_HELP_STRING([--with-lite-beapi-libs=],
                           [specify linker flags for Lite BeAPI])],
           [if test "x$with_lite_beapi_libs" = x; then
		spec_lite_beapi_libs=no
	    else
		spec_lite_beapi_libs=yes
	    fi],
	   [spec_lite_beapi_libs=no])

if test x$LITE_BEAPI_CONFIG = xno; then
	AC_PATH_PROG(LITE_BEAPI_CONFIG, lite-beapi-config, no)
fi

if test "$spec_lite_beapi_cflags" = "no" -o "$spec_lite_beapi_libs" = "no"; then
	have_lite_beapi=no
	if test x$LITE_BEAPI_CONFIG != xno ; then
		if $LITE_BEAPI_CONFIG --atleast-version 0.0.3 ; then
			LITE_BEAPI_CFLAGS="`$LITE_BEAPI_CONFIG --cflags`"
			LITE_BEAPI_LIBS="`$LITE_BEAPI_CONFIG --libs`"
			have_lite_beapi=yes
		fi
	fi

	if test "$have_lite_beapi" != "yes"; then
		AC_MSG_ERROR([
*** No Lite BeAPI found!
])
	fi
fi

if test "$spec_lite_beapi_cflags" = "yes"; then
	LITE_BEAPI_CFLAGS="$with_lite_beapi_cflags"
fi

if test "$spec_lite_beapi_libs" = "yes"; then
	LITE_BEAPI_LIBS="$with_lite_beapi_libs"
fi

AC_SUBST(LITE_BEAPI_CFLAGS)
AC_SUBST(LITE_BEAPI_LIBS)

CFLAGS="$CFLAGS $LITE_BEAPI_CFLAGS"
CXXFLAGS="$CXXFLAGS $LITE_BEAPI_CFLAGS"
LDFLAGS="$LDFLAGS $LITE_BEAPI_LIBS"

AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(LDFLAGS)

openwrt_build=no
AC_MSG_CHECKING(for OpenWrt)
case $target in
  *-openwrt-*)
    openwrt_build=yes
    ;;
esac
AC_MSG_RESULT($openwrt_build)
AM_CONDITIONAL(OPENWRT_BUILD, [test "$openwrt_build" = "yes"])

AC_CONFIG_FILES([
Makefile
include/Makefile
include/lbk/Makefile
libs/Makefile
libs/lbk/Makefile
libs/lbk/add-ons/Makefile
libs/lbk/add-ons/npi-oled-hat/Makefile
libs/lbk/add-ons/vpd/Makefile
cmd/Makefile
cmd/npi-oled-hat/Makefile
cmd/lcd-st7735s/Makefile
cmd/lbk/Makefile
cmd/scripts/Makefile
app/Makefile
app/panel/Makefile
])

AC_OUTPUT

