// Copyright (C) 2019,2020 Tomoyuki Fujimori <moyu@dromozoa.com>
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

    class value;

    struct table_type {
      mutex mutex;
      std::map<value, value> map;
    };

    class value {
    public:
      value() : type_(LUA_TNONE) {}
      value(lua_State*, int);
      value(const value&);

      ~value() {
        destruct_();
      }

      value& operator=(const value&);

      bool isnoneornil() const {
        return type_ == LUA_TNONE || type_ == LUA_TNIL;
      }

      void push(lua_State*) const;

      bool operator<(const value&) const;

    private:
      int type_;
      union {
        bool boolean_;
        double number_;
        std::string string_;
        std::shared_ptr<table_type> table_;
        void* userdata_;
      };

      void destruct_();
    };

    std::shared_ptr<table_type> table_to_map(lua_State* L, int arg) {
      std::shared_ptr<table_type> result = std::make_shared<table_type>();

      luaX_push(L, luaX_nil);
      while (lua_next(L, arg)) {
        value k(L, -2);
        value v(L, -1);
        if (!k.isnoneornil() && !v.isnoneornil()) {
          result->map[k] = v;
        }
        lua_pop(L, 1);
      }

      return result;
    }

    void map_new(lua_State* L, std::shared_ptr<table_type> source) {
      luaX_new<std::shared_ptr<table_type> >(L, source);
      luaX_set_metatable(L, "dromozoa.multi.map");
    }

    std::shared_ptr<table_type>* map_check(lua_State* L, int arg) {
      return luaX_check_udata<std::shared_ptr<table_type> >(L, arg, "dromozoa.multi.map");
    }

    void map_gc(lua_State* L) {
      map_check(L, 1)->~shared_ptr();
    }

    void map_get(lua_State* L) {
      try {
        std::shared_ptr<table_type> t = *map_check(L, 1);
        value k(L, 2);
        {
          lock_guard<> lock(t->mutex);
          std::map<value, value>::const_iterator i = t->map.find(k);
          if (i == t->map.end()) {
            luaX_push(L, luaX_nil);
          } else {
            i->second.push(L);
          }
        }
      } catch (const value_error& e) {
        luaL_argerror(L, e.arg(), e.msg());
      }
    }

    void map_set(lua_State* L) {
      try {
        std::shared_ptr<table_type> t = *map_check(L, 1);
        value k(L, 2);
        value v(L, 3);
        if (k.isnoneornil()) {
          throw value_error(2, "table index is nil");
        }
        {
          lock_guard<> lock(t->mutex);
          if (v.isnoneornil()) {
            t->map.erase(k);
          } else {
            t->map[k] = v;
          }
        }
      } catch (const value_error& e) {
        luaL_argerror(L, e.arg(), e.msg());
      }
    }

    value::value(lua_State* L, int arg) : type_(lua_type(L, arg)) {
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
            new (&string_) std::string(string.data(), string.size());
          }
          break;
        case LUA_TTABLE:
          new (&table_) std::shared_ptr<table_type>(table_to_map(L, arg));
          break;
        case LUA_TLIGHTUSERDATA:
          userdata_ = lua_touserdata(L, arg);
          break;
        default:
          throw value_error(arg, "nil/boolean/number/string/lightuserdata expected");
      }
    }

    value::value(const value& that) : type_(that.type_) {
      switch (type_) {
        case LUA_TNONE:
        case LUA_TNIL:
          break;
        case LUA_TBOOLEAN:
          boolean_ = that.boolean_;
          break;
        case LUA_TNUMBER:
          number_ = that.number_;
          break;
        case LUA_TSTRING:
          new (&string_) std::string(that.string_);
          break;
        case LUA_TTABLE:
          new (&table_) std::shared_ptr<table_type>(that.table_);
          break;
        case LUA_TLIGHTUSERDATA:
          userdata_ = that.userdata_;
          break;
        default:
          throw std::logic_error("unreachable code");
      }
    }

    void value::destruct_() {
      switch (type_) {
        case LUA_TSTRING:
          string_.~basic_string();
          break;
        case LUA_TTABLE:
          table_.~shared_ptr();
          break;
      }
      type_ = LUA_TNONE;
    }

    value& value::operator=(const value& that) {
      destruct_();
      type_ = that.type_;
      switch (type_) {
        case LUA_TNONE:
        case LUA_TNIL:
          break;
        case LUA_TBOOLEAN:
          boolean_ = that.boolean_;
          break;
        case LUA_TNUMBER:
          number_ = that.number_;
          break;
        case LUA_TSTRING:
          new (&string_) std::string(that.string_);
          break;
        case LUA_TTABLE:
          new (&table_) std::shared_ptr<table_type>(that.table_);
          break;
        case LUA_TLIGHTUSERDATA:
          userdata_ = that.userdata_;
          break;
        default:
          throw std::logic_error("unreachable code");
      }
      return *this;
    }

    void value::push(lua_State* L) const {
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
        case LUA_TTABLE:
          map_new(L, table_);
          break;
        case LUA_TLIGHTUSERDATA:
          lua_pushlightuserdata(L, userdata_);
          break;
        default:
          throw std::logic_error("unreachable code");
      }
    }

    bool value::operator<(const value& that) const {
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
        case LUA_TTABLE:
          return table_ < that.table_;
        case LUA_TLIGHTUSERDATA:
          return userdata_ < that.userdata_;
        default:
          throw std::logic_error("unreachable code");
      }
    }

    mutex env_mutex;
    std::map<value, value> env_map;

    void env_get(lua_State* L, int arg) {
      try {
        value k(L, arg);
        {
          lock_guard<> lock(env_mutex);
          std::map<value, value>::const_iterator i = env_map.find(k);
          if (i == env_map.end()) {
            luaX_push(L, luaX_nil);
          } else {
            i->second.push(L);
          }
        }
      } catch (const value_error& e) {
        luaL_argerror(L, e.arg(), e.msg());
      }
    }

    void env_set(lua_State* L, int arg) {
      try {
        value k(L, arg);
        value v(L, arg + 1);
        if (k.isnoneornil()) {
          throw value_error(arg, "table index is nil");
        }
        {
          lock_guard<> lock(env_mutex);
          if (v.isnoneornil()) {
            env_map.erase(k);
          } else {
            env_map[k] = v;
          }
        }
      } catch (const value_error& e) {
        luaL_argerror(L, e.arg(), e.msg());
      }
    }

    void impl_index(lua_State* L) {
      env_get(L, 2);
    }

    void impl_newindex(lua_State* L) {
      env_set(L, 2);
    }

    void impl_get(lua_State* L) {
      env_get(L, 1);
    }

    void impl_set(lua_State* L) {
      env_set(L, 1);
      luaX_push_success(L);
    }
  }

  void initialize_env(lua_State* L) {
    luaL_newmetatable(L, "dromozoa.multi.env");
    luaX_set_field(L, -1, "__index", impl_index);
    luaX_set_field(L, -1, "__newindex", impl_newindex);
    lua_pop(L, 1);

    lua_newuserdata(L, 0);
    luaX_set_metatable(L, "dromozoa.multi.env");
    luaX_set_field(L, -2, "env");

    luaX_set_field(L, -1, "get", impl_get);
    luaX_set_field(L, -1, "set", impl_set);

    luaL_newmetatable(L, "dromozoa.multi.map");
    luaX_set_field(L, -1, "__index", map_get);
    luaX_set_field(L, -1, "__newindex", map_set);
    luaX_set_field(L, -1, "__gc", map_gc);
    lua_pop(L, 1);
  }
}
