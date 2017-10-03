#***************************************************************************
# Testing configure options for nghttp2
#***************************************************************************

AC_INIT([hello], [1.0]) # good
AC_CHECK_HEADER([stdbool.h], [], [], [AC_INCLUDES_DEFAULT])

dnl ***************************************************************************
dnl nghttp2
dnl ***************************************************************************

NGHTTP2_CPPFLAGS=""
NGHTTP2_LDFLAGS=""
have_nghttp2="no"

# optional: HTTP/2 support. Disabled by default
AC_ARG_ENABLE([http2],
   [AS_HELP_STRING([--enable-http2],
               [enable HTTP/2 support (yes, no)[no]])],
   [enable_http2=${enableval}])
if test "x$enable_http2" = "xyes"
then
#
# Next block is large unindented block
#

# nghttp2
have_nghttp2_pkgcfg=no
AC_MSG_CHECKING([[how to find nghttp2 library]])
AC_ARG_WITH(nghttp2,
   [AS_HELP_STRING([[--with-nghttp2[=PFX]]],[use nghttp2 for HTTP/2 support, optional PFX overrides pkg-config data for nghttp2 headers (PFX/include) and libs (PFX/lib)])],
   [
    case $with_nghttp2 in
      no)
        AC_MSG_RESULT([[nghttp2 disabled]])
        ;;
      yes)
        AC_MSG_RESULT([[automatically, forced]])
        ;;
      *)
        AC_MSG_RESULT([[-I$with_nghttp2/include -L$with_nghttp2/lib -lnghttp2]])
        SAVE_LDFLAGS="$LDFLAGS"
        SAVE_CPPFLAGS="$CPPFLAGS"
        SAVE_LIBS="$LIBS"
        LDFLAGS="-L$with_nghttp2/lib $LDFLAGS"
        CPPFLAGS="-I$with_nghttp2/include $CPPFLAGS"
        have_nghttp2_pkgcfg=no

        # Code from Apache: httpd/modules/http2/config2.m4
        dnl initialise the variables we use
        have_nghttp2=no

        MOD_CFLAGS="$CPPFLAGS"
        MOD_LDFLAGS="$LDFLAGS"
        AC_MSG_CHECKING([for nghttp2 version >= 1.2.1])
        AC_TRY_COMPILE([#include <nghttp2/nghttp2ver.h>],[
 #if !defined(NGHTTP2_VERSION_NUM)
 #error "Missing nghttp2 version"
 #endif
 #if NGHTTP2_VERSION_NUM < 0x010201
 #error "Unsupported nghttp2 version " NGHTTP2_VERSION_TEXT
 #endif],
          [AC_MSG_RESULT(OK)
          have_nghttp2=yes],
          [AC_MSG_RESULT(FAILED)])

        AS_IF([test "x$have_nghttp2" != "xyes"], [AC_MSG_ERROR([can't find usable libnghttp2 at specified prefix $with_nghttp2])])

        if test "x$have_nghttp2" = "xyes"; then
          MOD_LDFLAGS="-lnghttp2 $MOD_LDFLAGS"
          LIBS="-lnghttp2 $LIBS"

          dnl Run library and function checks
          liberrors=""
          AC_CHECK_HEADERS([nghttp2/nghttp2.h])
          AC_CHECK_FUNCS([nghttp2_option_set_no_recv_client_magic], [], [liberrors="yes"])
          if test "x$liberrors" != "x"; then
            AC_MSG_WARN([nghttp2 library is unusable])
          fi
          AC_CHECK_FUNCS([nghttp2_session_server_new2], [], [liberrors="yes"])
          if test "x$liberrors" != "x"; then
            AC_MSG_WARN([nghttp2 library is unusable])
          fi
        dnl # nghttp2 >= 1.3.0: access to stream weights
              AC_CHECK_FUNCS([nghttp2_stream_get_weight], [], [liberrors="yes"])
              if test "x$liberrors" != "x"; then
                AC_MSG_WARN([nghttp2 version >= 1.3.0 is required])
              fi
        dnl # nghttp2 >= 1.5.0: changing stream priorities
              AC_CHECK_FUNCS([nghttp2_session_change_stream_priority],
                [MOD_CPPFLAGS="-DH2_NG2_CHANGE_PRIO $MOD_CPPFLAGS"], [])
        dnl # nghttp2 >= 1.14.0: invalid header callback
              AC_CHECK_FUNCS([nghttp2_session_callbacks_set_on_invalid_header_callback],
                [MOD_CPPFLAGS="-DH2_NG2_INVALID_HEADER_CB $MOD_CPPFLAGS"], [])
        dnl # nghttp2 >= 1.15.0: get/set stream window sizes
              AC_CHECK_FUNCS([nghttp2_session_get_stream_local_window_size],
                [MOD_CPPFLAGS="-DH2_NG2_LOCAL_WIN_SIZE $MOD_CPPFLAGS"], [])
            else
              AC_MSG_WARN([nghttp2 version is too old])
            fi
        if test "x$have_nghttp2" = "xyes"; then
          AC_DEFINE(HAVE_NGHTTP2, 1, [Define if nghttp2 is available])
        fi
        LIBS="$SAVE_LIBS"
        LDFLAGS="$SAVE_LDFLAGS"
        CPPFLAGS="$SAVE_CPPFLAGS"
        AS_IF([test "x$have_nghttp2" != "xyes"], [AC_MSG_ERROR([can't find usable libnghttp2 at specified prefix $with_nghttp2])])
        ;;
    esac
   ],
   [AC_MSG_RESULT([[automatically]])
   ])

#AS_IF([test "x$have_nghttp2" != "xyes" && test "x$with_nghttp2" = "xyes"], [AC_MSG_ERROR([[can't find usable libnghttp2]])])



# MSG_HTTP2="no (lacking libnghttp2)"
# MSG_HTTP2="yes (using libnghttp2)"
else
  MSG_HTTP2="no (disabled)"
  enable_http2="no"
fi
AC_MSG_CHECKING(whether to support HTTP/2)
AC_MSG_RESULT([$MSG_HTTP2])

# AM_CONDITIONAL([HAVE_NGHTTP2], [[test "x$have_nghttp2" = "xyes"]])
# AM_CONDITIONAL([ENABLE_HTTP2], [test "x$enable_http2" = "xyes"])


dnl ***************************************************************************

AC_MSG_NOTICE([${PACKAGE_NAME} ${PACKAGE_VERSION} Configuration Summary:
  HTTP/2 support:     ${MSG_HTTP2}
])
