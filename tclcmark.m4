AC_DEFUN(TCLCMARK_LOCATE_CMARK, [
	AC_ARG_WITH(cmark-gfm,
	    AS_HELP_STRING([--with-cmark-gfm=DIR],
		[installation directory of cmark-gfm]),
	    with_cmark_gfm="${withval}")
    AC_MSG_CHECKING([for cmark-gfm installation])
    HAVECMARK=0
    CMARK_LIBS=""
    if test x"${with_cmark_gfm}" != x ; then
        HAVECMARK=2
        CMARK_CFLAGS="-I${with_cmark_gfm}/include"
        if test x"${TEA_PLATFORM}" = x"windows" ; then
            CMARK_CFLAGS="$CMARK_CFLAGS -DCMARK_GFM_STATIC_DEFINE -DCMARK_GFM_EXTENSIONS_STATIC_DEFINE"
        fi
        CMARK_LIBS="${with_cmark_gfm}/lib/libcmark-gfm.a"
        # Note exensions lib must come before core library.
        if test -f "${with_cmark_gfm}/lib/libcmark-gfm-extensions.a"; then
            CMARK_LIBS="${with_cmark_gfm}/lib/libcmark-gfm-extensions.a $CMARK_LIBS"
        elif test -f "${with_cmark_gfm}/lib/libcmark-gfmextensions.a"; then
            CMARK_LIBS="${with_cmark_gfm}/lib/libcmark-gfmextensions.a $CMARK_LIBS"
        fi
	AC_MSG_RESULT([${with_cmark_gfm}])
    else
        HAVECMARK=`pkg-config --exists libcmark-gfm && echo "1"`
    fi

    if test "$HAVECMARK" = "1" ; then
        AC_MSG_RESULT([found])
        if test x"${TEA_PLATFORM}" = x"windows" ; then
            CMARK_CFLAGS="`pkg-config --cflags libcmark-gfm` -DCMARK_GFM_STATIC_DEFINE -DCMARK_GFM_EXTENSIONS_STATIC_DEFINE"
            CMARK_LIBS="-Wl,-Bstatic `pkg-config --static --libs libcmark-gfm` -lcmark-gfmextensions -Wl,-Bdynamic"
        else
            CMARK_CFLAGS="`pkg-config --cflags libcmark-gfm`"
            CMARK_LIBS="`pkg-config --libs libcmark-gfm`"
        fi
    fi

    if test x"${CMARK_LIBS}" = x ; then
        AC_MSG_ERROR([The required lib libcmark-gfm not found])
    fi
])
