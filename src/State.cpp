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

#include "State.h"
#include <exception>
#include <string>
#include <lua.hpp>
#include "ArgumentException.h"
#include "core.h"
#include "StateCallException.h"
#include "StateException.h"

namespace integral {
    State::State() : luaState_(luaL_newstate()) {
        if (luaState_ != nullptr) {
           lua_atpanic(getLuaState(), &State::atPanic);
        } else {
            throw StateException("Failed to allocate integral::State");
        }
    } 
    
    State::~State() {
        if (luaState_ != nullptr) {
            lua_close(luaState_);
        }
    }
    
    void State::doString(const std::string &string) const {
        if (luaL_dostring(getLuaState(), string.c_str()) != LUA_OK) {
            const std::string errorMessage(integral::get<std::string>(getLuaState(), -1));
            lua_pop(getLuaState(), 1);
            throw StateCallException(errorMessage);
        }
    }

    void State::doFile(const std::string &fileName) const {
        if (luaL_dofile(getLuaState(), fileName.c_str()) != LUA_OK) {
            const std::string errorMessage(integral::get<std::string>(getLuaState(), -1));
            lua_pop(getLuaState(), 1);
            throw StateCallException(errorMessage);
        }
    }

    int State::atPanic(lua_State *luaState) {
        try {
            throw StateException(std::string("lua panic error: ") + integral::get<std::string>(luaState, -1));
        } catch (const ArgumentException &argumentException) {
            throw StateException(std::string("lua panic error - could not retrieve error message from lua stack: ") + argumentException.what());
        } catch (const std::exception &exception) {
            throw StateException(std::string("lua panic error - miscellaneous exception thrown retrieving error message from lua stack: ") + exception.what());
        } catch (...) {
            throw StateException("lua panic error - unspecified exception thrown retrieving error message from lua stack");
        }
    }
}
