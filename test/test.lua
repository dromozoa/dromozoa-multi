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

local s1 = multi.state()
s1:loadbuffer [[
local unix = require "dromozoa.unix"
unix.nanosleep(0.4)
print "s1"
]]

local s2 = multi.state()
s2:loadbuffer [[
local unix = require "dromozoa.unix"
unix.nanosleep(0.2)
print "s2"
]]

print "start"

local t1 = multi.thread(s1)
local t2 = multi.thread(s2)

print "t2.join"
t2:join()
print "t1.join"
t1:join()
