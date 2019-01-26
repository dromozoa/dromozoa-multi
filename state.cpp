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

extern "C" {
#include <lualib.h>
}

namespace dromozoa {
  namespace {
    void impl_gc(lua_State* L) {
      check_state_handle(L, 1)->~state_handle();
    }

    void impl_call(lua_State* L) {
      state_handle self(luaL_newstate());
      if (self.get()) {
        luaL_openlibs(self.get());
        luaX_new<state_handle>(L, self.get());
        self.release();
        luaX_set_metatable(L, "dromozoa.multi.state");
      } else {
        luaX_throw_failure("cannot luaL_newstate");
      }
    }

    void impl_get(lua_State* L) {
      lua_pushlightuserdata(L, check_state_handle(L, 1)->get());
    }

    void impl_release(lua_State* L) {
      lua_pushlightuserdata(L, check_state_handle(L, 1)->release());
    }

    void impl_xcopy(lua_State* L) {
      luaX_push(L, check_state_handle(L, 1)->xcopy(L, 2));
    }

    void impl_load(lua_State* L) {
      lua_State* self = check_state(L, 1);
      if (!self) {
        luaX_throw_failure("invalid state");
      }
      luaX_string_reference chunk = luaX_check_string(L, 2);
      luaX_string_reference name;
      if (!lua_isnoneornil(L, 3)) {
        name = luaX_check_string(L, 3);
      }
      int result = luaL_loadbuffer(self, chunk.data(), chunk.size(), name.data());
      if (result == 0) {
        luaX_push_success(L);
      } else {
        luaX_throw_failure("cannot luaL_loadbuffer", result);
      }
    }

    void impl_loadfile(lua_State* L) {
      lua_State* self = check_state(L, 1);
      if (!self) {
        luaX_throw_failure("invalid state");
      }
      luaX_string_reference filename = luaX_check_string(L, 2);
      int result = luaL_loadfile(self, filename.data());
      if (result == 0) {
        luaX_push_success(L);
      } else {
        luaX_throw_failure("cannot luaL_loadfile", result);
      }
    }

    void impl_id(lua_State* L) {
      lua_State* self = check_state(L, 1);
      std::ostringstream out;
      out << self;
      luaX_push(L, out.str());
    }
  }

  state_handle* check_state_handle(lua_State* L, int arg) {
    return luaX_check_udata<state_handle>(L, arg, "dromozoa.multi.state");
  }

  lua_State* check_state(lua_State* L, int arg) {
    return check_state_handle(L, arg)->get();
  }

  void initialize_state(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.multi.state");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "get", impl_get);
      luaX_set_field(L, -1, "release", impl_release);
      luaX_set_field(L, -1, "xcopy", impl_xcopy);
      luaX_set_field(L, -1, "load", impl_load);
      luaX_set_field(L, -1, "loadfile", impl_loadfile);
      luaX_set_field(L, -1, "id", impl_id);
    }
    luaX_set_field(L, -2, "state");
  }
}
