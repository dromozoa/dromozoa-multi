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
    class value_error : std::exception {
    public:
      value_error(int arg, const char* what) : arg_(arg), what_(what) {}

      virtual ~value_error() throw() {}

      virtual const char* what() const throw() {
        return what_;
      }

    private:
      int arg_;
      const char* what_;
    };

    class value {
    public:
      value() : type_(LUA_TNIL) {}

      value(lua_State* L, int arg) : type_(LUA_TNIL) {
        switch (lua_type(L, arg)) {
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

      bool isnil() const {
        return type_ == LUA_TNIL;
      }

      bool operator<(const value& that) const {
        if (type_ != that.type_) {
          return type_ < that.type_;
        }
        switch (type_) {
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
      value k;
      value v;
      try {
        k = value(L, 1);
        v = value(L, 2);
      } catch (const value_error&) {
      }

        if (k.isnil()) {
          luaL_argerror(L, 1, "index is nil");
        }

    }

    void impl_get(lua_State* L) {
    }
  }

  void initialize_env(lua_State* L) {
    luaX_set_field(L, -1, "set", impl_set);
    luaX_set_field(L, -1, "get", impl_get);
  }
}
