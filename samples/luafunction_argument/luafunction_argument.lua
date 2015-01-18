--
--  luafunction_argument.lua
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

local algorithm = require("libalgorithm")
local vector = {1, 2, 3}
local doubleVector = algorithm.transform(vector, function(element) return 2*element end)
for i, v in ipairs(doubleVector) do
    print("[" .. i .. "]", v)
end
