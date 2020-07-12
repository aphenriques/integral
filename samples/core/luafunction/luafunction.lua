--
--  luafunction.lua
--  integral
--
-- MIT License
--
-- Copyright (c) 2014, 2016, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in all
-- copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
-- SOFTWARE.

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
