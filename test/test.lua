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

local chunk = [[
local unix = require "dromozoa.unix"

local n, t, v1, v2, v3 = ...
unix.nanosleep(t)
print(n, t, v1, v2, v3)
]]

local s1 = assert(multi.state():loadbuffer(chunk))
local s2 = assert(multi.state():loadbuffer(chunk))

print "start"
local t1 = assert(multi.thread(s1, 1, 0.4, nil, true, "foo"))
local t2 = assert(multi.thread(s2, 2, 0.2, false, "bar"))

print "t2.join"
t2:join()

print "t1.join"
t1:join()
