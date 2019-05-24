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
    class mutex_handle_impl {
    public:
      void add_ref() {
        ++count_;
      }

      void release() {
        if (--count_ == 0) {
          delete this;
        }
      }

      void lock() {
        mutex_.lock();
      }

      void unlock() {
        mutex_.unlock();
      }

    private:
      atomic_count<long> count_;
      mutex mutex_;
    };

    class mutex_handle {
    public:
      explicit mutex_handle(mutex_handle_impl* impl) : impl_(impl) {
        impl_->add_ref();
      }

      ~mutex_handle() {
        impl_->release();
      }

      mutex_handle_impl* share() {
        return impl_;
      }

      void lock() {
        impl_->lock();
      }

      void unlock() {
        impl_->unlock();
      }

    private:
      mutex_handle_impl* impl_;
    };

    mutex_handle* check_mutex_handle(lua_State* L, int arg) {
      return luaX_check_udata<mutex_handle>(L, arg, "dromozoa.multi.mutex");
    }

    void impl_gc(lua_State* L) {
      check_mutex_handle(L, 1)->~mutex_handle();
    }

    void impl_call(lua_State* L) {
      if (lua_islightuserdata(L, 2)) {
        luaX_new<mutex_handle>(L, static_cast<mutex_handle_impl*>(lua_touserdata(L, 2)));
        luaX_set_metatable(L, "dromozoa.multi.mutex");
      } else {
        scoped_ptr<mutex_handle_impl> impl(new mutex_handle_impl());
        luaX_new<mutex_handle>(L, impl.get());
        impl.release();
        luaX_set_metatable(L, "dromozoa.multi.mutex");
      }
    }

    void impl_share(lua_State* L) {
      lua_pushlightuserdata(L, check_mutex_handle(L, 1)->share());
    }

    void impl_lock(lua_State* L) {
      check_mutex_handle(L, 1)->lock();
      luaX_push_success(L);
    }

    void impl_unlock(lua_State* L) {
      check_mutex_handle(L, 1)->unlock();
      luaX_push_success(L);
    }
  }

  void initialize_mutex(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.multi.mutex");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__gc", impl_gc);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "share", impl_share);
      luaX_set_field(L, -1, "lock", impl_lock);
      luaX_set_field(L, -1, "unlock", impl_unlock);
    }
    luaX_set_field(L, -2, "mutex");
  }
}
