--
--  regex.lua
--  integral
--
-- MIT License
--
-- Copyright (c) 2013, 2014, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

local Regex = require("libRegex")

local pattern = Regex.new([[(.)\.(.)]]) -- literal string to avoid dealing with escape characters
local matches = pattern:match("4.2")
print("matches:", matches)
if matches then
    print("matches:getSize():", matches:getSize())
    for i = 0, matches:getSize() - 1 do
        print("matches("..i.."):", matches(i))
    end
end

print("---")

pattern = Regex.new("(.)")
matches = pattern:search("42")
print("matches:", matches)
-- maybe matches == nil. c++ std::regex implementation may be incomplete
if matches then
    print("matches:getSize():", matches:getSize())
    for i = 0, matches:getSize() - 1 do
        print("matches("..i.."):", matches(i))
    end
end

print("---")

-- should be nil
print("matches:", pattern:match("42"))
