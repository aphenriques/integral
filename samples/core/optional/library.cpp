//
//  library.cpp
//  integral
//
//  Copyright (C) 2019  André Pereira Henriques
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
#include <optional>
#include <lua.hpp>
#include <integral/integral.hpp>

extern "C" {
    LUALIB_API int luaopen_liblibrary(lua_State *luaState) {
        try {
            integral::push<integral::Table>(luaState);
            integral::setFunction(luaState, "getIdentity", [](std::optional<int> x) {
                return x;
            });
            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[optional sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[shared_ptr sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}

