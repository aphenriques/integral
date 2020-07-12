--
--  showcase.lua
--  integral
--
-- MIT License
--
-- Copyright (c) 2013, 2014, 2015, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

-- This sample does NOT show all integral features (only part of its capability)
local showcase = require("libshowcase")
-- no problem loading multiple integral libraries (no problem loading _any_ other library)

package.cpath = package.cpath .. ";otherlib/?.so;otherlib/?.dylib"
local otherlib = require("libotherlib")
print("-> libotherlib:")
print("otherlib.getMessage()", otherlib.getMessage())

print("\n-> Base class:")
baseObject = showcase.Base.new(42.1)
print("baseObject:getNumber():", baseObject:getNumber())
baseObject:setNumber(84.2)
print("baseObject:getNumber():", baseObject:getNumber())
-- number error due to float c type argument in c function (to avoid it, use double c type)
print("baseObject:getSum(21.1):", baseObject:getSum(21.1))

print("\n-> Derived class:")

derivedObject = showcase.Derived.new(-0.1, "string of Derived")
print("derivedObject:getNumber():", derivedObject:getNumber())
-- alternative way of calling method
print("showcase.Derived.getNumber(derivedObject):", showcase.Derived.getNumber(derivedObject))
print("derivedObject:getString():", derivedObject:getString())
print("derivedObject:getConcatenated(\" + extra string\"):", derivedObject:getConcatenated(" + extra string"))
print("derivedObject:__tostring():", derivedObject)
print("derivedObject:getNumberAndString():", derivedObject:getNumberAndString())

print("\n-> Inheritance and functions:")

-- number error due to float c type argument in c function (to avoid it, use double c type)
-- calling Base metatable class with a Derived object
print("showcase.Base.getSum(derivedObject, 21.1):", showcase.Base.getSum(derivedObject, 21.1))
-- or using inheritance:
 print("derivedObject:getSum(21.1)", derivedObject:getSum(21.1))
print("showcase.getSumBase(derivedObject, showcase.Base.new(-2)):getNumber():", showcase.getSumBase(derivedObject, showcase.Base.new(-2)):getNumber());
print("showcase.getSumBase(showcase.Base.new(-2), derivedObject):getNumber():", showcase.getSumBase(showcase.Base.new(-2), derivedObject):getNumber());

print("\n-> Error handling:")
local _, message = pcall(function() showcase.throwCppException("c++ exception test message") end)
print("Expected C++ exception error:", message)
local _, message = pcall(function() showcase.getSumBase(42) end)
print("Expected wrong function parameter error:", message)
local _, message = pcall(function() showcase.Derived.getString(baseObject) end)
print("Expected incompatible userdata (class) function parameter error:", message)
local _, message = pcall(function() showcase.Derived.getNumberAndString(nil) end)
print("Expected integral::get<T> error:", message)
local _, message = pcall(function() showcase.Derived.getNumberAndString(otherlib.getDummyObject()) end)
print("Expected incompatible usedata from other library error:", message)

print("\nEnd of showcase")
