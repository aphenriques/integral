//
//  ArgumentException.cpp
//  integral
//
//  Copyright (C) 2013, 2014, 2016  André Pereira Henriques
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

#include "ArgumentException.hpp"
#include <cstddef>
#include <cstring>
#include <sstream>
#include <lua.hpp>
#include "lua_compatibility.hpp"

namespace integral {
    // If a class is defined in a header file and has a vtable (either it has virtual methods or it derives from classes with virtual methods), it must always have at least one out-of-line virtual method in the class. Without this, the compiler will copy the vtable and RTTI into every .o file that #includes the header, bloating .o file sizes and increasing link times.
    // source: http://llvm.org/docs/CodingStandards.html#provide-a-virtual-method-anchor-for-classes-in-headers
    ArgumentException::~ArgumentException() {}

    ArgumentException ArgumentException::createTypeErrorException(lua_State *luaState, int index, const std::string &expectedTypeName) {
        const int gottenTypeCode = lua_type(luaState, index);
        const char * const gottenTypeName = lua_typename(luaState, gottenTypeCode);
        if (gottenTypeCode != LUA_TNUMBER || expectedTypeName != gottenTypeName) {
            return ArgumentException(luaState, index, expectedTypeName + " expected, got " + gottenTypeName);
        } else {
            return ArgumentException(luaState, index, "integral number type expected, got floating-point number");
        }
    }

    bool ArgumentException::findField(lua_State *luaState, int index, int level) {
        if (level == 0 || lua_istable(luaState, -1) == 0) {
            return false;
        }
        lua_pushnil(luaState);
        while (lua_next(luaState, -2) != 0) {
            if (lua_type(luaState, -2) == LUA_TSTRING) {
                if (lua_rawequal(luaState, index, -1) != 0) {
                    lua_pop(luaState, 1);
                    return true;
                } else if (findField(luaState, index, level - 1) == true) {
                    lua_remove(luaState, -2);
                    lua_pushliteral(luaState, ".");
                    lua_insert(luaState, -2);
                    lua_concat(luaState, 3);
                    return true;
                }
            }
            lua_pop(luaState, 1);
        }
        return false;
    }

    bool ArgumentException::pushGlobalFunctionName(lua_State *luaState, lua_Debug *debugInfo) {
        const int top = lua_gettop(luaState);
        lua_getinfo(luaState, "f", debugInfo);
        detail::lua_compatibility::pushglobaltable(luaState);
        if (findField(luaState, top + 1, 2) == true) {
            detail::lua_compatibility::copy(luaState, -1, top + 1);
            lua_pop(luaState, 2);
            return true;
        } else {
            lua_settop(luaState, top);
            return false;
        }
    }

    std::string ArgumentException::getExceptionMessage(lua_State *luaState, int index, const std::string &extraMessage) {
        lua_Debug debugInfo;
        if (lua_getstack(luaState, 0, &debugInfo) == 0) {
            std::ostringstream messageStream;
            messageStream << "bad argument #" << index << " (" << extraMessage << ")";
            return messageStream.str();
        }
        lua_getinfo(luaState, "n", &debugInfo);
        if (std::strcmp(debugInfo.namewhat, "method") == 0) {
            --index;
            if (index == 0) {
                std::ostringstream messageStream;
                messageStream << "calling '" << debugInfo.name << "' on bad self (" << extraMessage << ")";
                return messageStream.str();
            }
        }
        if (debugInfo.name == nullptr) {
            debugInfo.name = (pushGlobalFunctionName(luaState, &debugInfo) == true) ? lua_tostring(luaState, -1) : "?";
        }
        std::ostringstream messageStream;
        messageStream << "bad argument #" << index << " to '" <<  debugInfo.name << "' (" << extraMessage << ")";
        return messageStream.str();
    }

    std::string ArgumentException::getExceptionMessage(lua_State *luaState, std::size_t maximumNumberOfArguments, std::size_t actualNumberOfArguments) {
        lua_Debug debugInfo;
        if (lua_getstack(luaState, 0, &debugInfo) == 0) {
            std::ostringstream messageStream;
            messageStream << "excessive parameters provided to function (" << maximumNumberOfArguments << " expected, got " << actualNumberOfArguments << ")";
            return messageStream.str();
        }
        lua_getinfo(luaState, "n", &debugInfo);
        if (std::strcmp(debugInfo.namewhat, "method") == 0) {
            std::ostringstream messageStream;
            if (maximumNumberOfArguments > 0) {
                messageStream << "function or constructor '" << debugInfo.name << "' called as a method or excessive parameters provided to method '" << debugInfo.name << "' (" << (maximumNumberOfArguments - 1) << " expected, got " << (actualNumberOfArguments - 1) << ")";
            } else {
                messageStream << "function or constructor '" << debugInfo.name << "' called as a method";
            }
            return messageStream.str();
        }
        if (debugInfo.name == nullptr) {
            debugInfo.name = (pushGlobalFunctionName(luaState, &debugInfo) == true) ? lua_tostring(luaState, -1) : "?";
        }
        std::ostringstream messageStream;
        messageStream << "excessive parameters provided to function '" << debugInfo.name << "' (" << maximumNumberOfArguments << " expected, got " << actualNumberOfArguments << ")";
        return messageStream.str();
    }
}
