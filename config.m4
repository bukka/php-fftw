dnl $Id$
dnl config.m4 for extension fftw

PHP_ARG_WITH(fftw, for fftw support,
[  --with-fftw             Include fftw support])

if test "$PHP_FFTW" != "no"; then

  SEARCH_PATH="/usr/local /usr /local /opt"
  SEARCH_FOR="/include/fftw3.h"

  if test "$PHP_FFTW" = "yes"; then
    AC_MSG_CHECKING([for libfftw headers in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        FFTW_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  else 
    AC_MSG_CHECKING([for libfftw headers in $PHP_FFTW])
    if test -r $PHP_FFTW/$SEARCH_FOR; then
      FFTW_DIR=$PHP_FFTW
      AC_MSG_RESULT([found])
    fi
  fi

  if test -z "$FFTW_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Cannot find libfftw headers])
  fi

  PHP_ADD_INCLUDE($FFTW_DIR/include)

  LIBNAME=fftw3
  LIBSYMBOL=fftw_malloc

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $FFTW_DIR/$PHP_LIBDIR, FFTW_SHARED_LIBADD)
    AC_DEFINE(HAVE_FFTW,1,[Fann library found])
  ],[
    AC_MSG_ERROR([wrong libfftw version or lib not found])
  ],[
    -L$FFTW_DIR/$PHP_LIBDIR -lm
  ])

  PHP_SUBST(FFTW_SHARED_LIBADD)

  PHP_NEW_EXTENSION(fftw, fftw.c, $ext_shared)
fi

