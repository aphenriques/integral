--
--  table_conversion.lua
--  integral
--
-- MIT License
--
-- Copyright (c) 2014, 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

local adaptors = require("libtable_conversion")

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

print("# std::vector")
local vectorTable = adaptors.getVectorSample()
print(type(vectorTable))
print(getStringFromTable(vectorTable))
print("double vector: " .. getStringFromTable(adaptors.getDoubleVector(vectorTable)))
adaptors.printVectorOfVectors({{11, 12, 13}, {21, 22}, {31}})
-- wrong element type error handling
local _, message = pcall(function() adaptors.printVectorOfVectors({{1}, 2}) end)
print("invalid vector element error: " .. message)
print("--")

print("# std::array")
local arrayTable = adaptors.getArrayOf3VectorsSample()
print(type(arrayTable))
print(getStringFromTable(arrayTable))
adaptors.printArrayOf2Numbers({1, 2})
-- wrong number of elements error handling
local _, message = pcall(function() adaptors.printArrayOf2Numbers({1}) end)
print("wrong array size error: " .. message)
print("--")

print("# std::unordered_map")
local unorderedMapTable = adaptors.getUnorderedMapSample()
print(type(unorderedMapTable))
print(getStringFromTable(unorderedMapTable))
adaptors.printStringDoubleUnorderedMap({pi = math.pi, rad = math.rad(1), 42})
print("--")

print("# std::tuple")
local tuple = adaptors.getTupleSample()
print(type(tuple))
print(getStringFromTable(tuple))
adaptors.printBoolBoolTuple({true, false})
local _, message = pcall(function() adaptors.printBoolBoolTuple({}) end)
print("wrong tuple size error: " .. message)
print("--")
