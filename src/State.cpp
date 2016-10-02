//
//  State.cpp
//  integral
//
//  Copyright (C) 2016  André Pereira Henriques
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

#include "State.hpp"
#include <exception>
#include <string>
#include <lua.hpp>
#include "exception/ClassException.hpp"
#include "ArgumentException.hpp"
#include "core.hpp"

namespace integral {
    State::State() : luaState_(luaL_newstate(), [](lua_State *luaState) -> void {
        if (luaState != nullptr) {
            lua_close(luaState);
        }
    }) {
        if (luaState_.get() != nullptr) {
           lua_atpanic(getLuaState(), &State::atPanic);
        } else {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "[integral] failed to allocate integral::State");
        }
    }

    void State::doString(const std::string &string) const {
        if (luaL_dostring(getLuaState(), string.c_str()) != LUA_OK) {
            try {
                throw StateException(getErrorMessage(getLuaState()));
            } catch (...) {
                lua_pop(getLuaState(), 1);
                throw;
            }
        }
    }

    void State::doFile(const std::string &fileName) const {
        if (luaL_dofile(getLuaState(), fileName.c_str()) != LUA_OK) {
            try {
                throw StateException(getErrorMessage(getLuaState()));
            } catch (...) {
                lua_pop(getLuaState(), 1);
                throw;
            }
        }
    }
    
    // stack argument: errorString
    std::string State::getErrorMessage(lua_State *luaState) {
        try {
            return integral::get<std::string>(luaState, -1);
        } catch (const ArgumentException &argumentException) {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string("[integral] could not retrieve error message from lua stack: ") + argumentException.what());
        } catch (const std::exception &exception) {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string("[integral] miscellaneous exception thrown retrieving error message from lua stack: ") + exception.what());
        } catch (...) {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "[integral] unspecified exception thrown retrieving error message from lua stack");
        }
    }

    int State::atPanic(lua_State *luaState) {
        try {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string("[integral] lua panic error: ") + getErrorMessage(luaState));
        } catch (...) {
            lua_pop(luaState, 1);
            throw;
        }
    }
}
