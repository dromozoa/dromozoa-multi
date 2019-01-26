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
    thread* check_thread(lua_State* L, int arg) {
      return luaX_check_udata<thread>(L, arg, "dromozoa.multi.thread");
    }

    void* start_routine(void* arg) {
      state_handle that(static_cast<lua_State*>(arg));
      int nargs = lua_tonumber(that.get(), -1);
      lua_pop(that.get(), 1);
      if (lua_pcall(that.get(), nargs, 0, 0) != 0) {
        DROMOZOA_UNEXPECTED(lua_tostring(that.get(), -1));
      }
      return 0;
    }

    void impl_gc(lua_State* L) {
      check_thread(L, 1)->~thread();
    }

    void impl_call(lua_State* L) {
      state_handle* that = check_state_handle(L, 2);
      if (!that->get()) {
        luaX_throw_failure("invalid state");
      }
      int nargs = that->xcopy(L, 3);
      luaX_push(that->get(), nargs);
      try {
        luaX_new<thread>(L, start_routine, that->get());
        that->release();
        luaX_set_metatable(L, "dromozoa.multi.thread");
      } catch (const system_error& e) {
        luaX_throw_failure(e.what(), e.code());
      }
    }

    void impl_detach(lua_State* L) {
      try {
        check_thread(L, 1)->detach();
        luaX_push_success(L);
      } catch (const system_error& e) {
        luaX_throw_failure(e.what(), e.code());
      }
    }

    void impl_join(lua_State* L) {
      try {
        check_thread(L, 1)->join();
        luaX_push_success(L);
      } catch (const system_error& e) {
        luaX_throw_failure(e.what(), e.code());
      }
    }

    void impl_id(lua_State* L) {
      thread* self = check_thread(L, 1);
      std::ostringstream out;
      out << self->get_id();
      luaX_push(L, out.str());
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
      luaX_set_field(L, -1, "id", impl_id);
    }
    luaX_set_field(L, -2, "thread");
  }
}
