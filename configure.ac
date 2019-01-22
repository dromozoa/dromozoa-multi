# Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
#
# This file is part of dromozoa-multi.
#
# dromozoa-multi is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# dromozoa-multi is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with dromozoa-multi.  If not, see <http://www.gnu.org/licenses/>.

m4_include([version.m4])
AC_INIT([dromozoa-multi], DROMOZOA_MULTI_VERSION, [moyu@dromozoa.com], [], [https://github.com/dromozoa/dromozoa-multi/])
AM_INIT_AUTOMAKE
LT_INIT

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile])

AC_PROG_CXX

AX_PTHREAD([], [AC_MSG_ERROR([could not find pthread])])
CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS"
LIBS="$LIBS $PTHREAD_LIBS"
AC_SEARCH_LIBS([pthread_create], [pthread])

AX_PROG_LUA([5.1], [], [], [AC_MSG_ERROR([could not find lua])])
AX_LUA_HEADERS([], [AC_MSG_ERROR([could not find lua])])
AX_LUA_LIBS([], [AC_MSG_ERROR([could not find lua])])

AC_OUTPUT
