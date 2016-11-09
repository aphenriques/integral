//
//  StateView.cpp
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

#include "StateView.hpp"
#include <exception>
#include <string>
#include <lua.hpp>
#include "exception/Exception.hpp"
#include "ArgumentException.hpp"
#include "core.hpp"

namespace integral {
    StateView::StateView(lua_State *luaState) : luaState_(luaState) {
        if (luaState_ != nullptr) {
            lua_atpanic(getLuaState(), &StateView::atPanic);
        } else {
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, "[integral] nullptr passed to StateView::StateView(lua_State *luaState)");
        }
    }

    void StateView::doString(const std::string &string) const {
        if (luaL_dostring(getLuaState(), string.c_str()) != LUA_OK) {
            std::string errorMessage;
            try {
                errorMessage = integral::get<std::string>(getLuaState(), -1);
            } catch (const ArgumentException &argumentException) {
                lua_pop(getLuaState(), 1);
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string(kErrorStackArgument) + argumentException.what() + " }");
            } catch (const std::exception &exception) {
                lua_pop(getLuaState(), 1);
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string(kErrorStackMiscellanous) + exception.what() + " }");
            } catch (...) {
                lua_pop(getLuaState(), 1);
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, kErrorStackUnspecified);
            }
            lua_pop(getLuaState(), 1);
            throw StateException(__FILE__, __LINE__, __func__, std::string("[integral] ") + errorMessage);
        }
    }

    void StateView::doFile(const std::string &fileName) const {
        if (luaL_dofile(getLuaState(), fileName.c_str()) != LUA_OK) {
            std::string errorMessage;
            try {
                errorMessage = integral::get<std::string>(getLuaState(), -1);
            } catch (const ArgumentException &argumentException) {
                lua_pop(getLuaState(), 1);
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string(kErrorStackArgument) + argumentException.what() + " }");
            } catch (const std::exception &exception) {
                lua_pop(getLuaState(), 1);
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string(kErrorStackMiscellanous) + exception.what() + " }");
            } catch (...) {
                lua_pop(getLuaState(), 1);
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, kErrorStackUnspecified);
            }
            lua_pop(getLuaState(), 1);
            throw StateException(__FILE__, __LINE__, __func__, std::string("[integral] ") + errorMessage);
        }
    }

    int StateView::atPanic(lua_State *luaState) {
        std::string errorMessage;
        try {
            errorMessage = integral::get<std::string>(luaState, -1);
        } catch (const ArgumentException &argumentException) {
            lua_pop(luaState, 1);
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string(kErrorStackArgument) + argumentException.what() + " }");
        } catch (const std::exception &exception) {
            lua_pop(luaState, 1);
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string(kErrorStackMiscellanous) + exception.what() + " }");
        } catch (...) {
            lua_pop(luaState, 1);
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, kErrorStackUnspecified);
        }
        lua_pop(luaState, 1);
        throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string("[integral] lua panic error: ") + errorMessage);
    }

    const char * const StateView::kErrorStackArgument = "[integral] could not retrieve error message from lua stack: { ";
    const char * const StateView::kErrorStackMiscellanous = "[integral] miscellaneous exception thrown retrieving error message from lua stack: { ";
    const char * const StateView::kErrorStackUnspecified = "[integral] unspecified exception thrown retrieving error message from lua stack";
}
