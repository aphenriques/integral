//
//  exchanger.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2013, 2014, 2016, 2019, 2020, 2022, 2023 André Pereira Henriques (aphenriques (at) outlook (dot) com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "exchanger.hpp"
#include <cstddef>
#include <string>
#include <lua.hpp>
#include "ArgumentException.hpp"
#include "type_manager.hpp"


namespace integral {
    namespace detail {
        namespace exchanger {
            const char * const gkAutomaticInheritanceKey = "integral_AutomaticInheritanceKey";

            const char * Exchanger<const char *>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    const char * const string = lua_tostring(luaState, index);
                    if (string != nullptr) {
                        return string;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TSTRING));
                    }
                } else {
                    const char **userData = type_manager::getConvertibleType<const char *>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TSTRING));
                    }
                }
            }

            bool Exchanger<bool>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    if (lua_isboolean(luaState, index) != 0) {
                        return lua_toboolean(luaState, index) != 0;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TBOOLEAN));
                    }
                } else {
                    const bool *userData = type_manager::getConvertibleType<bool>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TBOOLEAN));
                    }
                }
            }

            const char * const Exchanger<LuaFunctionWrapper>::kMetatableName_ = "integral_LuaFunctionWrapperMetatableName";

            LuaFunctionWrapper Exchanger<LuaFunctionWrapper>::get(lua_State *luaState, int index) {
                if (lua_iscfunction(luaState, index) != 0) {
                    if (lua_getupvalue(luaState, index, 1) != nullptr) {
                        // stack: upvalue
                        const LuaFunctionWrapper *luaFunctionWrapperPointer = basic::getAlignedObjectPointer<LuaFunctionWrapper>(
                            luaState,
                            -1,
                            kMetatableName_
                        );
                        if (luaFunctionWrapperPointer != nullptr) {
                            LuaFunctionWrapper luaFunctionWrapper = *luaFunctionWrapperPointer;
                            lua_pop(luaState, 1);
                            // stack:
                            return luaFunctionWrapper;
                        } else {
                            const ArgumentException argumentException = ArgumentException::createTypeErrorException(luaState, index, "LuaFunctionWrapper");
                            lua_pop(luaState, 1);
                            // stack:
                            throw argumentException;
                        }
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, "LuaFunctionWrapper (no upvalue)");
                    }
                } else {
                    throw ArgumentException::createTypeErrorException(luaState, index, "lua_CFuntion");
                }
            }

            std::string Exchanger<LuaFunctionWrapper>::getCurrentSourceAndLine(lua_State *luaState) {
                lua_Debug debugInfo;
                if (lua_getstack(luaState, 1, &debugInfo) != 0) {
                    lua_getinfo(luaState, "S", &debugInfo);
                    lua_getinfo(luaState, "l", &debugInfo);
                    return std::string(debugInfo.short_src) + ':' + std::to_string(debugInfo.currentline);
                } else {
                    throw exception::LogicException(__FILE__, __LINE__, __func__, "level greater than the stack depth");
                }
            }
        }
    }
}
