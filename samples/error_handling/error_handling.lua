--
--  error_handling.lua
--  integral
--
--  Copyright (C) 2013, 2014  André Pereira Henriques
--  aphenriques (at) outlook (dot) com
--
--  This file is part of integral.
--
--  integral is free software: you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation, either version 3 of the License, or
--  (at your option) any later version.
--
--  integral is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with integral.  If not, see <http://www.gnu.org/licenses/>.
--

local Object = require("libObject")

local _, message = pcall(function() Object.new("prefix", 42) end)
print("Constructor argument error message (1): ", message)
local _, message = pcall(function() Object.new() end)
print("Constructor argument error message (2): ", message)
-- correct expression
local o1 = Object.new("o1_string")

local _, message = pcall(function() o1.getString(42) end)
print("Function argument error message (1): ", message)
local _, message = pcall(function() o1:getString(42) end)
print("Function argument error message (2): ", message)
-- correct expression
o1:getString()

local _, message = pcall(function() o1:createSuffixedObjectLuaFunction(o1) end)
print("Lua Function argument error message: ", message)
-- correct exception
 o1:createSuffixedObjectLuaFunction("_suffix")

local _, message = pcall(function() Object.throwCppException() end)
print("Thrown exception message: ", message)
