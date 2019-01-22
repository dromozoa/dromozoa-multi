-- Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
--
-- This file is part of dromozoa-multi.
--
-- dromozoa-multi is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- dromozoa-multi is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with dromozoa-multi.  If not, see <http://www.gnu.org/licenses/>.

local multi = require "dromozoa.multi"

local verbose = os.getenv "VERBOSE" == "1"

local this_thread_id = assert(multi.this_thread_id())
local this_state_id = assert(multi.this_state_id())

if verbose then
  io.write(this_thread_id, ",", this_state_id, "\n")
end

local s = assert(multi.state():load [[
local multi = require "dromozoa.multi"

local out = assert(io.open("test.txt", "w"))
out:write(multi.this_thread_id(), ",", multi.this_state_id())
out:close()
]])
local state_id = s:id()

local t = assert(multi.thread(s))
local thread_id = t:id()

t:join()

if verbose then
  io.write(thread_id, ",", state_id, "\n")
end

local handle = assert(io.open("test.txt"))
assert(handle:read "*a" == thread_id .. "," .. state_id)
handle:close()

os.remove "test.txt"
