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
local unix = require "dromozoa.unix"

assert(#multi.mutex == 8)
for i = 1, 8 do
  print(multi.mutex[i]:native_handle())
end

local mutex = multi.mutex[1]

local chunk = [[
local multi = require "dromozoa.multi"
local unix = require "dromozoa.unix"

assert(#multi.mutex == 8)
for i = 1, 8 do
  print(multi.mutex[i]:native_handle())
end

local mutex = multi.mutex[1]

print "lock t2"
mutex:lock()
print "locked t2"
unix.nanosleep(0.5)
print "unlock t2"
mutex:unlock()
]]

print "lock t1"
mutex:lock()
print "locked t1"

local s = assert(multi.state():load(chunk))
local t = assert(multi.thread(s))

unix.nanosleep(0.5)
print "unlock t1"
mutex:unlock()

print "lock t1"
mutex:lock()
print "locked t1"
unix.nanosleep(0.5)
print "unlock t1"
mutex:unlock()

t:join()
