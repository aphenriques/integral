//
//  otherlib.cpp
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

#include <functional>
#include <iostream>
#include <string>
#include <lua.hpp>
#include "integral.h"

class DummyObject {};

extern "C" {
    LUALIB_API int luaopen_libotherlib(lua_State *luaState) {
        try {
            lua_newtable(luaState);
            // stack: table (module table)

            integral::setFunction(luaState, "getMessage", []() -> std::string {
                return "message from otherlib";
            });

            // DummyObject is automatically registered
            integral::setFunction(luaState, "getDummyObject", []() -> DummyObject {
                return DummyObject();
            });

            // Do not forget the return value (number of values to be returned from stack)!
            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[otherlib sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[otherlib sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}
