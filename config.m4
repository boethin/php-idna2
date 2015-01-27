dnl $Id$
dnl config.m4 for extension idna2

PHP_ARG_WITH(idn, for libidn support, [  --with-idna2            Include idna2 support])

if test "$PHP_IDNA2" != "no"; then

  dnl check libidn path
  SEARCH_PATH="/usr/local /usr"
  SEARCH_FOR="/include/idna.h"
  if test -r $PHP_IDNA2/$SEARCH_FOR; then
    LIBIDN_DIR=$PHP_IDNA2
  else
    AC_MSG_CHECKING([for libidn in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        LIBIDN_DIR=$i
        AC_MSG_RESULT(found in $i)
        break
      fi
    done
  fi

  if test -z "$LIBIDN_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the GNU libidn distribution])
  fi

  dnl add libidn include path
  PHP_ADD_INCLUDE($LIBIDN_DIR/lib)

  dnl check for libidn and symbol presence
  LIBNAME=idn
  PHP_CHECK_LIBRARY($LIBNAME,stringprep_check_version,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LIBIDN_DIR/lib, IDNA2_SHARED_LIBADD)
    AC_DEFINE(HAVE_IDNA2LIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong libidn version or libidn not found])
  ],[
    -L$LIBIDN_DIR/lib -lm -ldl
  ])


  dnl check idnkit-2.0 path
  SEARCH_PATH="/usr/local /usr"
  SEARCH_FOR="/include/idn/api.h"
  if test -r $PHP_IDNA2/$SEARCH_FOR; then
    IDNKIT2_DIR=$PHP_IDNA2
  else
    AC_MSG_CHECKING([for idnkit-2.0 in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        IDNKIT2_DIR=$i
        AC_MSG_RESULT(found in $i)
        break
      fi
    done
  fi

  if test -z "$IDNKIT2_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the idnkit-2.0 library])
  fi

  dnl add idnkit-2.0 include path
  PHP_ADD_INCLUDE($IDNKIT2_DIR/include)

  dnl check for idnkit-2.0 and symbol presence
  LIBNAME=idnkitlite
  PHP_CHECK_LIBRARY($LIBNAME, idn_version_getstring,
  [
     PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $IDNKIT2_DIR/lib, IDNA2_SHARED_LIBADD)
    AC_DEFINE(HAVE_IDNKIT2, 1, [ ])
  ],[
    AC_MSG_ERROR([wrong idnkit-2.0 version or idnkit-2.0 not found])
  ],[
  ])


  PHP_SUBST(IDNA2_SHARED_LIBADD)
  PHP_NEW_EXTENSION(idna2, idna2.c idna2/idna2.c idna2/libidn.c idna2/idnkit2.c, $ext_shared)
fi
