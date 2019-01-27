// Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
//
// This file is part of dromozoa-multi.
//
// dromozoa-multi is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dromozoa-multi is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dromozoa-multi.  If not, see <http://www.gnu.org/licenses/>.

#include "common.hpp"

namespace dromozoa {
  void initialize_env(lua_State*);
  void initialize_main(lua_State*);
  void initialize_state(lua_State*);
  void initialize_thread(lua_State*);

  void initialize(lua_State* L) {
    initialize_env(L);
    initialize_main(L);
    initialize_state(L);
    initialize_thread(L);
  }
}

extern "C" int luaopen_dromozoa_multi(lua_State* L) {
  lua_newtable(L);
  dromozoa::initialize(L);
  return 1;
}
