--
--  luafunction.lua
--  integral
--
--  Copyright (C) 2014, 2016  André Pereira Henriques
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

-- MacOX specific shared library extension
package.cpath = package.cpath .. ";?.dylib"

Object = require("libObject")

o1 = Object.new("prefix")
print(o1:getString())
o2 = o1:createSuffixedObjectLuaFunction("_suffix")
print(o2:getString())
o2:addSuffixLuaFunction("_end")
print(o2:getString())

print("--")
-- argument checking is performed by integral::get
print("wrong parameters handling", pcall(function() Object.createSuffixedObjectLuaFunction("wrong parameters") end))

print("--")
Object.printUpvalue()
printer = Object.getPrinter("object_printer")
printer()

print("--")
Object.printMessage()

print("--")
Object.printStack();
