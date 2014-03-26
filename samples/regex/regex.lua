--
--  regex.lua
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
-- maybe matches == nill. c++ std::regex implementation may be incomplete
if matches then
    print("matches:getSize():", matches:getSize())
    for i = 0, matches:getSize() - 1 do
        print("matches("..i.."):", matches(i))
    end
end

print("---")

-- should be nill
print("matches:", pattern:match("42"))
