-- Copyright (C) 2019,2020 Tomoyuki Fujimori <moyu@dromozoa.com>
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

assert(multi.get "foo" == nil)
assert(multi.set("foo", false))
assert(multi.get "foo" == false)
assert(multi.set("foo", 42))
assert(multi.get "foo" == 42)
assert(multi.set("foo", "bar"))
assert(multi.get "foo" == "bar")
assert(multi.set "foo")
assert(multi.get "foo" == nil)

-- local result, message = pcall(multi.set, {}, "baz")
-- if verbose then
--   print(message)
-- end
-- assert(not result)

-- local result, message = pcall(multi.set, "baz", {})
-- if verbose then
--   print(message)
-- end
-- assert(not result)

local result, message = pcall(multi.set, nil, "baz")
if verbose then
  print(message)
end
assert(not result)

local env = assert(multi.env)
if verbose then
  print(env)
end
assert(env.foo == nil)
env.foo = false
assert(env.foo == false)
env.foo = 42
assert(env.foo == 42)
env.foo = "bar"
assert(env.foo == "bar")
env.foo = nil
assert(env.foo == nil)

env.foo = false
env.bar = 42
env.baz = "qux"

env.big = ("abcdefghijklmnopqrstuvwxyz"):rep(65536)

env.m = {
  foo = 17;
  bar = true;
  baz = "qux";
}

local m = env.m
local t = multi.map_to_table(m)
if verbose then
  print(m)
  print(m.foo)
  print(m.bar)
  print(m.baz)

  print(t)
  print(t.foo)
  print(t.bar)
  print(t.baz)
end
m = nil

collectgarbage()
collectgarbage()

local s = multi.state():load [[
local multi = require "dromozoa.multi"
local env = multi.env

local verbose = os.getenv "VERBOSE" == "1"
if verbose then
  print(env.foo, env.bar, env.baz)
end

assert(multi.get "foo" == false)
assert(multi.get "bar" == 42)
assert(multi.get "baz" == "qux")

assert(env.foo == false)
assert(env.bar == 42)
assert(env.baz == "qux")
assert(env.m.baz == "qux")
env.m = nil
]]

multi.thread(s, 1):join()

if verbose then
  print "done"
end

env.m = {
  foo = { 1, 2, 3, 4 };
  bar = { { { 42 } } };
}

print(env.m.bar[1][1][1])

local t = multi.map_to_table(env.m)
print(t.foo)
print(t.bar)

env.x = {}
local x = env.x

for i = 1, 32 do
  local s = ("x"):rep(i)
  x[s] = s
end

for i = 1, 32 do
  local s = ("x"):rep(i)
  if verbose then
    print(i, x[s], s)
  end
  assert(x[s] == s)
end
