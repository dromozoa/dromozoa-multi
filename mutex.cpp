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

#include <dromozoa/bind/atomic.hpp>
#include <dromozoa/bind/mutex.hpp>

namespace dromozoa {
  namespace {
    class mutex_handle {
    public:
      explicit mutex_handle(mutex* mutex) : mutex_(mutex) {}

      void lock() {
        mutex_->lock();
      }

      void unlock() {
        mutex_->unlock();
      }

      pthread_mutex_t* native_handle() {
        return mutex_->native_handle();
      }

    private:
      mutex* mutex_;
    };

    mutex_handle* check_mutex_handle(lua_State* L, int arg) {
      return luaX_check_udata<mutex_handle>(L, arg, "dromozoa.multi.mutex");
    }

    void impl_gc(lua_State* L) {
      check_mutex_handle(L, 1)->~mutex_handle();
    }

    void impl_lock(lua_State* L) {
      check_mutex_handle(L, 1)->lock();
      luaX_push_success(L);
    }

    void impl_unlock(lua_State* L) {
      check_mutex_handle(L, 1)->unlock();
      luaX_push_success(L);
    }

    void impl_native_handle(lua_State* L) {
      lua_pushlightuserdata(L, check_mutex_handle(L, 1)->native_handle());
    }

    mutex mutex1;
    mutex mutex2;
    mutex mutex3;
    mutex mutex4;
    mutex mutex5;
    mutex mutex6;
    mutex mutex7;
    mutex mutex8;
  }

  void initialize_mutex(lua_State* L, mutex* mutex, int index) {
    int top = lua_gettop(L);
    luaX_new<mutex_handle>(L, mutex);
    luaX_set_metatable(L, "dromozoa.multi.mutex");
    luaX_set_field(L, top, index);
  }

  void initialize_mutex(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.multi.mutex");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_field(L, -1, "lock", impl_lock);
      luaX_set_field(L, -1, "unlock", impl_unlock);
      luaX_set_field(L, -1, "native_handle", impl_native_handle);

      initialize_mutex(L, &mutex1, 1);
      initialize_mutex(L, &mutex2, 2);
      initialize_mutex(L, &mutex3, 3);
      initialize_mutex(L, &mutex4, 4);
      initialize_mutex(L, &mutex5, 5);
      initialize_mutex(L, &mutex6, 6);
      initialize_mutex(L, &mutex7, 7);
      initialize_mutex(L, &mutex8, 8);
    }
    luaX_set_field(L, -2, "mutex");
  }
}
