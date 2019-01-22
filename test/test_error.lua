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

local s = multi.state()
local result, message, code = s:load "!!!! cannot compile !!!!"
if verbose then
  print(message, code)
end
assert(not result)
assert(code)
assert(code ~= 0)

local s = multi.state()
local result, message, code = s:loadfile "!!!! no such file !!!!"
if verbose then
  print(message, code)
end
assert(not result)
assert(code)
assert(code ~= 0)

local s = multi.state():load [[
local token = ...
local out = assert(io.open("test.txt", "w"))
out:write(token)
out:close()
]]
local result, message = pcall(function ()
  multi.thread(s, 42, {})
end)
if verbose then
  print(message)
end
assert(not result)

local token = os.date "%Y-%m-%d %H:%M:%S"
local t = multi.thread(s, token)

local result, message = multi.thread(s)
if verbose then
  print(message)
end
assert(not result)

assert(t:join())

local handle = assert(io.open("test.txt"))
assert(handle:read "*a" == token)
handle:close()

os.remove "test.txt"
