# Copyright (C) 2019,2020 Tomoyuki Fujimori <moyu@dromozoa.com>
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

CPPFLAGS += -Ibind -I$(LUA_INCDIR)
CXXFLAGS += -std=c++11 -Wall -W $(CFLAGS)
LDLIBS += -lpthread -ldl

OBJS = \
	env.o \
	main.o \
	module.o \
	mutex.o \
	state.o \
	state_handle.o \
	thread.o
TARGET = multi.so

all: $(TARGET)

clean:
	rm -f *.o $(TARGET)

check:
	./test.sh

multi.so: $(OBJS)
	$(CXX) $(LDFLAGS) $(LIBFLAG) $^ $(LDLIBS) -o $@

.cpp.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

install:
	mkdir -p $(LIBDIR)/dromozoa
	cp $(TARGET) $(LIBDIR)/dromozoa
