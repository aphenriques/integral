--
--  adaptors.lua
--  integral
--
--  Copyright (C) 2014  André Pereira Henriques
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

local adaptors = require("libadaptors")

local function getStringFromTable(table)
    local returnString = "{"
    for k, v in pairs(table) do
        if type(k) == "string" then
            returnString = returnString .. " [\"" .. k .. "\"]="
        else
            returnString = returnString .. " [" .. k .. "]="
        end
        if type(v) ~= "table" then
            returnString = returnString .. v
        else
            returnString = returnString .. getStringFromTable(v)
        end
    end
    returnString = returnString .. " }"
    return returnString
end

print("# LuaVector")
local vectorTable = adaptors.getVectorSample()
print(type(vectorTable))
print(getStringFromTable(vectorTable))
adaptors.printVectorOfVectors({{11, 12, 13}, {21, 22}, {31}})
-- wrong element type error handling
local _, message = pcall(function() adaptors.printVectorOfVectors({{1}, 2}) end)
print("invalid vector element error: " .. message)
print("--")

print("# LuaArray")
local arrayTable = adaptors.getArrayOf3VectorsSample()
print(type(arrayTable))
print(getStringFromTable(arrayTable))
adaptors.printArrayOf2Numbers({1, 2})
-- wrong number of elements error handling
local _, message = pcall(function() adaptors.printArrayOf2Numbers({1}) end)
print("wrong array size error: " .. message)
print("--")

print("# LuaUnorderedMap")
local unorderedMapTable = adaptors.getUnorderedMapSample()
print(type(unorderedMapTable))
print(getStringFromTable(unorderedMapTable))
adaptors.printStringDoubleUnorderedMap({pi = math.pi, rad = math.rad(1), 42})
print("--")

print("# LuaTuple")
local tuple = adaptors.getTupleSample()
print(type(tuple))
print(getStringFromTable(tuple))
adaptors.printBoolBoolTuple({true, false})
local _, message = pcall(function() adaptors.printBoolBoolTuple({}) end)
print("wrong tuple size error: " .. message)
print("--")

print("# LuaPack")
local int, bool, string = adaptors.getPackSample()
print(int, bool, string)
adaptors.printBoolBoolPack(true, false)
