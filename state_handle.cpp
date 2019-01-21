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
  state_handle::state_handle(lua_State* state) : state_(state) {}

  state_handle::~state_handle() {
    if (state_) {
      lua_close(state_);
    }
  }

  lua_State* state_handle::get() const {
    return state_;
  }

  lua_State* state_handle::release() {
    lua_State* state = state_;
    state_ = 0;
    return state;
  }
}
