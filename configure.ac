#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([congratbot],[0.1],[caoimhe@caoimhe.io])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([main.cc])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_RANLIB
AC_PROG_CXX
AC_CHECK_PROGS(PROTOC, [protoc])

PKG_CHECK_MODULES([PROTOBUF], [protobuf >= 2.4.0])
AC_SUBST(PROTOBUF_LIBS)
AC_SUBST(PROTOBUF_CFLAGS)
AC_SUBST(PROTOBUF_VERSION)

AC_LANG(C++)

OLDCXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -std=c++20"
AC_MSG_CHECKING([whether $CXX accepts -std=c++20])
AC_LINK_IFELSE([AC_LANG_CALL([], [main])], [AC_MSG_RESULT(yes)], [AC_MSG_RESULT(no); CXXFLAGS="$OLDCXXFLAGS"])
unset OLDCXXFLAGS
AC_SUBST(AC_CXXFLAGS)

# Checks for libraries.
AC_CHECK_LIB([protobuf], [main], [AC_LIBS="-lprotobuf $AC_LIBS"], AC_MSG_ERROR(libprotobuf is required))
AC_CHECK_LIB([mastodonpp], [main], [AC_LIBS="-lmastodonpp $AC_LIBS"], AC_MSG_ERROR(libmastodonpp is required))
AC_CHECK_LIB([jsoncpp], [main], [AC_LIBS="-ljsoncpp $AC_LIBS"], AC_MSG_ERROR(libjsoncpp is required))
LIBS="$LIBS $AC_LIBS"
AC_LIBS=""
AC_SUBST(AC_LIBS)
AC_SUBST(LIBS)

# Checks for header files.
AC_CHECK_HEADER([google/protobuf/text_format.h], [], AC_MSG_ERROR(protobuf headers not found))
AC_CHECK_HEADER([json/json.h], [], AC_MSG_ERROR(jsoncpp headers not found))
AC_CHECK_HEADER([mastodonpp/mastodonpp.hpp], [], AC_MSG_ERROR(mastodonpp headers not found))

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
