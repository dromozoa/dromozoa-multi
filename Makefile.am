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

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	.gitignore \
	README.md \
	bind \
	docs \
	dromozoa-multi-*.rockspec \
	m4/update \
	test \
	$(TESTS)
TESTS = test.sh

luaexec_LTLIBRARIES = multi.la

noinst_HEADERS = common.hpp

multi_la_CPPFLAGS = -I$(top_srcdir)/bind
multi_la_LDFLAGS = -module -avoid-version -shared
multi_la_SOURCES = \
	main.cpp \
	state.cpp \
	state_handle.cpp \
	module.cpp \
	thread.cpp
