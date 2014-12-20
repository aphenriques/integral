//
//  algorithm.cpp
//  integral
//
//  Copyright (C) 2013, 2014  André Pereira Henriques
//  aphenriques (at) outlook (dot) com
//
//  This file is part of integral.
//
//  integral is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  integral is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with integral.  If not, see <http://www.gnu.org/licenses/>.
//

#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <lua.hpp>
#include "integral.h"

namespace {
    integral::LuaVector<int> transform(integral::LuaVector<int> luaVectorCopy, const integral::LuaFunctionArgument &luaFunctionArgument) {
        // luaVectorCopy is a copy of the lua vector
        // the lua table argument is not modified
        std::for_each(luaVectorCopy.begin(), luaVectorCopy.end(), [&luaFunctionArgument](int &element) -> void {
            element = luaFunctionArgument.call<int>(element);
        });
        return luaVectorCopy;
    }
}

extern "C" {
    LUALIB_API int luaopen_libalgorithm(lua_State *luaState) {
        try {
            // module table
            lua_newtable(luaState);
            integral::setFunction(luaState, "transform", transform);
            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[luafunction_argument sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[luafunction_argument sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}
