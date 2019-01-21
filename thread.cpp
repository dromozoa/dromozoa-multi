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
  namespace {
    void* start_routine(void* data) {
      state_handle that(static_cast<lua_State*>(data));
      if (lua_pcall(that.get(), 0, 0, 0) != 0) {
        DROMOZOA_UNEXPECTED(lua_tostring(that.get(), -1));
      }
      return 0;
    }

    thread* check_thread(lua_State* L, int arg) {
      return luaX_check_udata<thread>(L, arg, "dromozoa.multi.thread");
    }

    void impl_gc(lua_State* L) {
      check_thread(L, 1)->~thread();
    }

    void impl_call(lua_State* L) {
      state_handle* that = check_state_handle(L, 2);
      luaX_new<thread>(L, start_routine, that->get());
      luaX_set_metatable(L, "dromozoa.multi.thread");
      that->release();
    }

    void impl_detach(lua_State* L) {
      thread* self = check_thread(L, 1);
      self->detach();
      luaX_push_success(L);
    }

    void impl_join(lua_State* L) {
      thread* self = check_thread(L, 1);
      self->join();
      luaX_push_success(L);
    }
  }

  void initialize_thread(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.multi.thread");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "detach", impl_detach);
      luaX_set_field(L, -1, "join", impl_join);
    }
    luaX_set_field(L, -2, "thread");
  }
}
