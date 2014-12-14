--
--  default_argument.lua
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

-- MacOX specific shared library extension
package.cpath = package.cpath .. ";?.dylib"

Object = require("libObject")

object = Object.new()
print("object = Object.new() -- default constructor")
print("object:getString():")
print(object:getString())
print("--")
object = Object.new("object_string")
print([[object = Object.new("object_string")]])
print("object:getString():")
print(object:getString())
print("--")
print("Object.testDefault(nil, nil):")
Object.testDefault(nil, nil)
print("object:testDefault():")
object:testDefault()
print("object:testDefault(4):")
object:testDefault(4)
print("Object.testDefault(nil, 2):")
Object.testDefault(nil, 2)
print("--")
print("Object.testDefault2(nil, \"testDefault2 string argument\"):")
Object.testDefault2(nil, "testDefault2 string argument")
