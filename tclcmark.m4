AC_DEFUN(TCLCMARK_LOCATE_CMARK, [
	AC_ARG_WITH(cmark-gfm,
	    AC_HELP_STRING([--with-cmark-gfm=DIR],
		[installation directory of cmark-gfm]),
	    with_cmark_gfm="${withval}")
    AC_MSG_CHECKING([for cmark-gfm installation])
    HAVECMARK=0
    CMARK_LIBS=""
    if test x"${with_cmark_gfm}" != x ; then
        HAVECMARK=2
        CMARK_CFLAGS="-I${with_cmark_gfm}/include"
        CMARK_LIBS="${with_cmark_gfm}/lib/libcmark-gfm-extensions.a ${with_cmark_gfm}/lib/libcmark-gfm.a"
	AC_MSG_RESULT([${with_cmark_gfm}])
    else
        HAVECMARK=`pkg-config --exists libcmark-gfm && echo "1"`
    fi

    if test "$HAVECMARK" = "1" ; then
        AC_MSG_RESULT([found])
        if test "${TEA_PLATFORM}" = "windows" ; then
            CMARK_CFLAGS="`pkg-config --cflags libcmark-gfm`"
            CMARK_LIBS="-Wl,-Bstatic `pkg-config --static --libs libcmark-gfm` -lcmark-gfmextensions -Wl,-Bdynamic"
        else
            CMARK_CFLAGS="`pkg-config --cflags libcmark-gfm`"
            CMARK_LIBS="`pkg-config --libs libcmark-gfm` -lcmark-gfmextensions"
        fi
    fi

    if test x"${CMARK_LIBS}" = x ; then
        AC_MSG_ERROR([The required lib libcmark-gfm not found])
    fi
])
