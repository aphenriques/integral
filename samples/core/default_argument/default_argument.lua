--
--  default_argument.lua
--  integral
--
-- MIT License
--
-- Copyright (c) 2014, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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
