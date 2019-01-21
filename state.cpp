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
      if (lua_State* state = luaL_newstate()) {
        luaX_new<state_handle>(L, state);
        luaX_set_metatable(L, "dromozoa.multi.state");
      } else {
        luaX_throw_failure("cannot luaL_newstate");
      }
    }

    void impl_openlibs(lua_State* L) {
      lua_State* state = check_state(L, 1);
      luaL_openlibs(state);
      luaX_push_success(L);
    }

    void impl_loadbuffer(lua_State* L) {
      lua_State* state = check_state(L, 1);
      luaX_string_reference chunk = luaX_check_string(L, 2);
      luaX_string_reference name;
      if (!lua_isnoneornil(L, 3)) {
        name = luaX_check_string(L, 3);
      }
      int result = luaL_loadbuffer(state, chunk.data(), chunk.size(), name.data());
      if (result == 0) {
        luaX_push_success(L);
      } else {
        luaX_throw_failure("cannot luaL_loadbuffer", result);
      }
    }

    void impl_loadfile(lua_State* L) {
      lua_State* state = check_state(L, 1);
      luaX_string_reference filename = luaX_check_string(L, 2);
      int result = luaL_loadfile(state, filename.data());
      if (result == 0) {
        luaX_push_success(L);
      } else {
        luaX_throw_failure("cannot luaL_loadfile", result);
      }
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
      luaX_set_field(L, -1, "openlibs", impl_openlibs);
      luaX_set_field(L, -1, "loadbuffer", impl_loadbuffer);
      luaX_set_field(L, -1, "loadfile", impl_loadfile);
    }
    luaX_set_field(L, -2, "state");
  }
}
