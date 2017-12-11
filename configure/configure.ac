#***************************************************************************
# Testing configure options for nghttp2
#***************************************************************************

AC_INIT([hello], [1.0]) # good
AC_CHECK_HEADER([stdbool.h], [], [], [AC_INCLUDES_DEFAULT])

# AC_DEFUN(PKG_CHECK_MODULES, [])

AC_DEFUN([AM_CONDITIONAL], [if $2; then $1=true; else $1=false; fi])
dnl ***************************************************************************
dnl nghttp2
dnl ***************************************************************************

NGHTTP2_CPPFLAGS=""
NGHTTP2_LDFLAGS=""
have_nghttp2="no"

# optional: HTTP2 support. Disabled by default
AC_ARG_ENABLE([http2],
   [AS_HELP_STRING([--enable-http2],
               [enable HTTP2 support (yes, no)[no]])],
   [enable_http2=${enableval}])
if test "x$enable_http2" = "xyes"
then
  # nghttp2
  have_nghttp2_pkgcfg=no
  want_http2=yes
  AC_MSG_CHECKING([[how to find nghttp2 library]])
  AC_ARG_WITH(nghttp2,
     [AS_HELP_STRING([[--with-nghttp2[=PFX]]],[use nghttp2 for HTTP2 support, optional PFX overrides pkg-config data for nghttp2 headers (PFX/include) and libs (PFX/lib)])],
     [
      case $with_nghttp2 in
        no)
          AC_MSG_RESULT([[nghttp2 disabled]])
          want_http2=no
          ;;
        yes)
          AC_MSG_RESULT([[automatically, forced]])
          want_http2=yes
          ;;
        *)
          AC_MSG_RESULT([[-I$with_nghttp2/include -L$with_nghttp2/lib -lnghttp2]])
          want_http2=yes
          try_with_nghttp2=yes
          ;;
      esac
     ],
     [AC_MSG_RESULT([[automatically]])
     want_http2=yes
     ])

  dnl Check nghttp2 version
  AC_DEFUN([NGHTTP2_CHECK], [
    have_nghttp2=no
    # curl: curl/configure.ac
    # use nghttp2_option_set_no_recv_client_magic to require nghttp2 >= 1.0.0
    AC_CHECK_LIB(nghttp2, nghttp2_option_set_no_recv_client_magic,
      [
       AC_CHECK_HEADERS(nghttp2/nghttp2.h,
          NGHTTP2_CPPFLAGS="-I$with_nghttp2/include"
          NGHTTP2_LDFLAGS="-L$with_nghttp2/lib"
          NGHTTP2_LIBS="-lnghttp2"
          have_nghttp2=yes
       )
      ],
    )

    # Apache: httpd/modules/http2/config2.m4
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
    AS_IF([test "x$have_nghttp2" != "xyes"], [AC_MSG_ERROR([cannot find usable libnghttp2 at specified prefix $with_nghttp2])])

    if test "x$have_nghttp2" = "xyes"; then
      dnl Run library and function checks
      liberrors=""
    dnl # use nghttp2_option_set_no_recv_client_magic to require nghttp2 >= 1.0.0 (curl)
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
            [NGHTTP2_CPPFLAGS="-DH2_NG2_CHANGE_PRIO $NGHTTP2_CPPFLAGS"], [])
    dnl # nghttp2 >= 1.14.0: invalid header callback
          AC_CHECK_FUNCS([nghttp2_session_callbacks_set_on_invalid_header_callback],
            [NGHTTP2_CPPFLAGS="-DH2_NG2_INVALID_HEADER_CB $NGHTTP2_CPPFLAGS"], [])
    dnl # nghttp2 >= 1.15.0: get/set stream window sizes
          AC_CHECK_FUNCS([nghttp2_session_get_stream_local_window_size],
            [NGHTTP2_CPPFLAGS="-DH2_NG2_LOCAL_WIN_SIZE $NGHTTP2_CPPFLAGS"], [])
    else
      AC_MSG_WARN([nghttp2 version is too old])
    fi
  ]) dnl NGHTTP2_CHECK

  # Check for nghttp2 at given path
  AS_IF([test "x$try_with_nghttp2" = "xyes"],
    [
      SAVE_LDFLAGS="$LDFLAGS"
      SAVE_CPPFLAGS="$CPPFLAGS"
      SAVE_LIBS="$LIBS"
      LDFLAGS="-L$with_nghttp2/lib $LDFLAGS"
      CPPFLAGS="-I$with_nghttp2/include $CPPFLAGS"
      NGHTTP2_CHECK()
      AS_IF([test "x$have_nghttp2" != "xyes"], [AC_MSG_ERROR([cannot find usable libnghttp2 at specified prefix $with_nghttp2])])
    ], [])

  # Check for nghttp2 at default paths
  AS_IF([test "x$want_http2" != "xno" && test "x$have_nghttp2" != "xyes"],
    [
      PKG_CHECK_MODULES(NGHTTP2, [[libnghttp2]],
        [
         have_nghttp2_pkgcfg='yes'
         SAVE_CPPFLAGS="$CPPFLAGS"
         SAVE_CFLAGS="$CFLAGS"
         SAVE_LDFLAGS="$LDFLAGS"
         SAVE_LIBS="$LIBS"
         CPPFLAGS="$NGHTTP2_CFLAGS $CPPFLAGS"
         CFLAGS="$NGHTTP2_CFLAGS $CFLAGS"
         LDFLAGS="$NGHTTP2_LIBS $LDFLAGS"
         LIBS="$LIBS $NGHTTP2_LIBS"
         AC_MSG_CHECKING([[whether nghttp2 is usable]])
         NGHTTP2_CHECK()
         AS_IF([test "x$have_nghttp2" = "xyes"],
           [
            AC_MSG_RESULT([[yes]])
            have_nghttp2=yes
            NGHTTP2_CPPFLAGS="$NGHTTP2_CFLAGS"
            NGHTTP2_LDFLAGS="$NGHTTP2_LIBS"
           ],
           [
            AC_MSG_RESULT([[no]])
            have_nghttp2=no
           ]
         )
        ],
        [
         AS_IF([test "x$have_nghttp2" != "xyes"], [AC_MSG_WARN([pkg-config reports that nghttp2 is present, but nghttp2 cannot be used])])
         CPPFLAGS="$SAVE_CPPFLAGS"
         CFLAGS="$SAVE_CFLAGS"
         LDFLAGS="$SAVE_LDFLAGS"
         LIBS="$SAVE_LIBS"
        ], [])
    ], []
  )

  AS_IF([test "x$want_http2" != "xno" && test "x$have_nghttp2" != "xyes"],
  [
    # check for nghttp2 at default paths
    have_nghttp2_pkgcfg='no'
    NGHTTP2_CHECK()
    AS_IF([test "x$have_nghttp2" = "xyes"],
      [
       AC_MSG_RESULT([[yes]])
       have_nghttp2=yes
       NGHTTP2_CPPFLAGS="$NGHTTP2_CFLAGS"
       NGHTTP2_LDFLAGS="$NGHTTP2_LIBS"
      ],
      [
       AC_MSG_RESULT([[no]])
       have_nghttp2=no
      ]
    )
  ])
  AS_IF([test "x$have_nghttp2" != "xyes" && test "x$with_nghttp2" = "xyes"], [AC_MSG_ERROR([[cannot find usable libnghttp2]])])

  AC_SUBST([NGHTTP2_CPPFLAGS])
  AC_SUBST([NGHTTP2_CFLAGS])
  AC_SUBST([NGHTTP2_LDFLAGS])
  AC_SUBST([NGHTTP2_LIBS])

  AS_IF([[test "x$have_nghttp2" = "xyes"]],
    [
      AC_DEFINE([[USE_NGHTTP2]], [[1]], [Define if nghttp2 is available])
      AC_DEFINE([[HTTP2_SUPPORT]], [[1]], [Define to 1 if libmicrohttpd is compiled with HTTP2 support.])
      enable_http2=yes
      MSG_HTTP2="yes (using libnghttp2)"
      MHD_LIB_CPPFLAGS="$MHD_LIB_CPPFLAGS $NGHTTP2_CPPFLAGS"
      MHD_LIB_CFLAGS="$MHD_LIB_CFLAGS $NGHTTP2_CFLAGS"
      MHD_LIB_LDFLAGS="$MHD_LIB_LDFLAGS $NGHTTP2_LDFLAGS"
      MHD_LIBDEPS="$NGHTTP2_LIBS $MHD_LIBDEPS"

      AS_IF([[ test "x$have_nghttp2_pkgcfg" = "xyes" ]],
        [ # remove nghttp2 from private libs in .pc file as it defined in Requires.private
          MHD_REQ_PRIVATE='nghttp2'
        ],
        [
          MHD_REQ_PRIVATE=''
          MHD_LIBDEPS_PKGCFG="$NGHTTP2_LIBS $MHD_LIBDEPS_PKGCFG"
        ])
    ],
    [
     AS_IF([[test "x$enable_http2" = "xyes" ]], [AC_MSG_ERROR([[HTTP2 support cannot be enabled without libnghttp2.]])])
     enable_http2=no
     MSG_HTTP2="no (lacking libnghttp2)"
    ]
  )
else
  MSG_HTTP2="no (disabled)"
fi

AC_MSG_CHECKING(whether to support HTTP2)
AC_MSG_RESULT([$MSG_HTTP2])

AM_CONDITIONAL([HAVE_NGHTTP2], [[test "x$have_nghttp2" = "xyes"]])
AM_CONDITIONAL([ENABLE_HTTP2], [test "x$enable_http2" = "xyes"])

dnl ***************************************************************************

AC_MSG_NOTICE([${PACKAGE_NAME} ${PACKAGE_VERSION} Configuration Summary:
  HTTP2 support:     ${MSG_HTTP2}
  http2 MHD_LIB_CPPFLAGS=$MHD_LIB_CPPFLAGS
  http2 MHD_LIB_LDFLAGS=$MHD_LIB_LDFLAGS
  http2 MHD_LIB_CFLAGS=$MHD_LIB_CFLAGS
  http2 MHD_LIBDEPS=$MHD_LIBDEPS
  http2 HAVE_NGHTTP2=$HAVE_NGHTTP2
  http2 ENABLE_HTTP2=$ENABLE_HTTP2
  http2 HTTP2_SUPPORT=$HTTP2_SUPPORT
])
