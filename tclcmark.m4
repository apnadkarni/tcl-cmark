AC_DEFUN(TCLCMARK_LOCATE_CMARK, [
    AC_MSG_CHECKING([for cmark-gfm installation])
    CMARK_LIBS=""
    HAVECMARK=`pkg-config --exists libcmark-gfm && echo "1"`
    if test "$HAVECMARK" = "1" ; then
        AC_MSG_RESULT([found])
        if test "${TEA_PLATFORM}" = "windows" ; then
            CMARK_LIBS="-Wl,-Bstatic `pkg-config --static --cflags --libs libcmark-gfm` -Wl,-Bdynamic"
        else
            CMARK_LIBS="`pkg-config --cflags --libs libcmark-gfm`"
        fi
    else
        AC_MSG_ERROR([The required lib libcmark-gfm not found])
    fi
])
