//
//  multiple_return.cpp
//  integral
//
//  Copyright (C) 2014, 2015  André Pereira Henriques
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

#include <iostream>
#include <string>
#include <lua.hpp>
#include "integral.h"

integral::LuaPack<std::string, double, unsigned> getSamplePack1() {
    return {"foo", -1.2, 42};
}

extern "C" {
    LUALIB_API int luaopen_libmultiple_return(lua_State *luaState) {
        try {
            lua_newtable(luaState);
            integral::setFunction(luaState, "getSamplePack1", getSamplePack1);

            integral::setFunction(luaState, "getSamplePack2", std::function<integral::LuaPack<int, int, int>()>([]() -> integral::LuaPack<int, int, int> {
                return {1, 2, 3};
            }));

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[multiple_return sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[multiple_return sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}
