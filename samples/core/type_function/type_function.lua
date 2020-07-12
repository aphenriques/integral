--
--  type_function.lua
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

Container = require("libContainer")

container = Container.new(42.1)
print("Container.getNumberFromId(container):", Container.getNumberFromId(container)) -- Container type conversion to Id
print("container:getNumberFromId():", container:getNumberFromId()) -- alternative construct
print("Container.getNegativeNumber(container):", Container.getNegativeNumber(container)) -- Container type conversion to double
print("container:getNegativeNumber():", container:getNegativeNumber()) -- alternative construct
