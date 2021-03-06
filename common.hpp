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

#ifndef DROMOZOA_COMMON_HPP
#define DROMOZOA_COMMON_HPP

#include <dromozoa/bind.hpp>

namespace dromozoa {
  class state_handle {
  public:
    explicit state_handle(lua_State*);
    ~state_handle();
    lua_State* get() const;
    lua_State* release();
    int xcopy(lua_State*, int);
  private:
    lua_State* state_;
    state_handle(const state_handle&);
    state_handle& operator=(const state_handle&);
  };

  state_handle* check_state_handle(lua_State*, int);
  lua_State* check_state(lua_State*, int);
}

#endif
