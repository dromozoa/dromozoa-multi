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

#include <map>
#include <stdexcept>
#include <string>

#include <dromozoa/bind/mutex.hpp>

namespace dromozoa {
  namespace {
    class value_error {
    public:
      value_error(int arg, const char* msg) : arg_(arg), msg_(msg) {}

      int arg() const {
        return arg_;
      }

      const char* msg() const {
        return msg_;
      }

    private:
      int arg_;
      const char* msg_;
    };

    class value {
    public:
      value() : type_(LUA_TNONE) {}

      value(lua_State* L, int arg) : type_(lua_type(L, arg)) {
        switch (type_) {
          case LUA_TNONE:
          case LUA_TNIL:
            break;
          case LUA_TBOOLEAN:
            boolean_ = lua_toboolean(L, arg);
            break;
          case LUA_TNUMBER:
            number_ = lua_tonumber(L, arg);
            break;
          case LUA_TSTRING:
            {
              luaX_string_reference string = luaX_to_string(L, arg);
              string_.assign(string.data(), string.size());
            }
            break;
          case LUA_TLIGHTUSERDATA:
            userdata_ = lua_touserdata(L, arg);
            break;
          default:
            throw value_error(arg, "nil/boolean/number/string/lightuserdata expected");
        }
      }

      bool isnoneornil() const {
        return type_ == LUA_TNONE || type_ == LUA_TNIL;
      }

      void push(lua_State* L) const {
        switch (type_) {
          case LUA_TNONE:
          case LUA_TNIL:
            luaX_push(L, luaX_nil);
            break;
          case LUA_TBOOLEAN:
            luaX_push(L, boolean_);
            break;
          case LUA_TNUMBER:
            luaX_push(L, number_);
            break;
          case LUA_TSTRING:
            luaX_push(L, string_);
            break;
          case LUA_TLIGHTUSERDATA:
            lua_pushlightuserdata(L, userdata_);
            break;
          default:
            throw std::logic_error("unreachable code");
        }
      }

      bool operator<(const value& that) const {
        if (type_ != that.type_) {
          return type_ < that.type_;
        }
        switch (type_) {
          case LUA_TNONE:
          case LUA_TNIL:
            return false;
          case LUA_TBOOLEAN:
            return boolean_ < that.boolean_;
          case LUA_TNUMBER:
            return number_ < that.number_;
          case LUA_TSTRING:
            return string_ < that.string_;
          case LUA_TLIGHTUSERDATA:
            return userdata_ < that.userdata_;
          default:
            throw std::logic_error("unreachable code");
        }
      }

    private:
      int type_;
      union {
        bool boolean_;
        double number_;
        void* userdata_;
      };
      std::string string_;
    };

    mutex env_mutex;
    std::map<value, value> env_map;

    void impl_set(lua_State* L) {
      try {
        value k = value(L, 1);
        value v = value(L, 2);
        if (k.isnoneornil()) {
          throw value_error(1, "table index is nil");
        }

        lock_guard<> lock(env_mutex);
        if (v.isnoneornil()) {
          env_map.erase(k);
        } else {
          env_map[k] = v;
        }
        luaX_push_success(L);
      } catch (const value_error& e) {
        luaL_argerror(L, e.arg(), e.msg());
      }
    }

    void impl_get(lua_State* L) {
      try {
        value k = value(L, 1);

        lock_guard<> lock(env_mutex);
        std::map<value, value>::const_iterator i = env_map.find(k);
        if (i == env_map.end()) {
          luaX_push(L, luaX_nil);
        } else {
          i->second.push(L);
        }
      } catch (const value_error& e) {
        luaL_argerror(L, e.arg(), e.msg());
      }
    }
  }

  void initialize_env(lua_State* L) {
    // luaL_newmetatable(L, "dromozoa.multi.env");
    // luaX_set_field(L, -1, "__index", impl_index);
    // luaX_set_field(L, -1, "__newindex", impl_newindex);

    // lua_newtable(L);
    // {
    //   luaX_set_metafield(L, -1, "__index", impl_index);
    //   luaX_set_metafield(L, -1, "__newindex", impl_newindex);
    // }
    // luaX_set_field(L, -2, "env");

    luaX_set_field(L, -1, "set", impl_set);
    luaX_set_field(L, -1, "get", impl_get);
  }
}
