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

#include <sstream>

#include <dromozoa/bind/thread.hpp>

namespace dromozoa {
  namespace {
    void impl_this_thread_id(lua_State* L) {
      std::ostringstream out;
      out << this_thread::get_id();
      luaX_push(L, out.str());
    }

    void impl_this_state_id(lua_State* L) {
      std::ostringstream out;
      out << L;
      luaX_push(L, out.str());
    }
  }

  void initialize_main(lua_State* L) {
    luaX_set_field(L, -1, "this_thread_id", impl_this_thread_id);
    luaX_set_field(L, -1, "this_state_id", impl_this_state_id);
  }
}
