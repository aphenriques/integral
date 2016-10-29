//
//  LuaFunctionWrapper.hpp
//  integral
//
//  Copyright (C) 2013, 2014, 2015, 2016  André Pereira Henriques
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

#ifndef integral_LuaFunctionWrapper_hpp
#define integral_LuaFunctionWrapper_hpp

#include <functional>
#include <utility>
#include <lua.hpp>
#include "exchanger.hpp"

namespace integral {
    namespace detail {
        // This class exists to make it possible to get exceptions thrown by luaFunction_ (see Exchanger<LuaFunctionWrapper>::push). For example, this is important to get exceptions thrown by exchanger::callConstructor when there are wrong parameters (exchanger::Exchange throws).
        // The function is managed by integral so that if an exception is thrown from it, it is translated to a Lua error
        // The first upvalue in the related lua_CFunction is always the userdata that holds LuaFunctionWrapper, so the remaining upvalues indexes are offset by 1.
        // Use getUpValueIndex or lua_upvalueindex(index + 1) to get an upvalue index.
        class LuaFunctionWrapper {
        public:
            // Gets adjusted upvalue index.
            // Exchanger<LuaFunctionWrapper>::push offsets the upvalues to insert the bound function userdata in the first position.
            static inline int getUpValueIndex(int index);

            // "luaFunction": lua_CFunction like function or functor.
            template<typename T>
            inline LuaFunctionWrapper(T &&luaFunction);

            // necessary because of the template constructor
            LuaFunctionWrapper(const LuaFunctionWrapper &) = default;
            LuaFunctionWrapper(LuaFunctionWrapper &) = default;
            LuaFunctionWrapper(LuaFunctionWrapper &&) = default;

            inline const std::function<int(lua_State *)> & getLuaFunction() const;

        private:
            std::function<int(lua_State *)> luaFunction_;
        };

        namespace exchanger {
            template<>
            class Exchanger<LuaFunctionWrapper> {
            public:
                static LuaFunctionWrapper get(lua_State *luaState, int index);

                // Attention: nUpValues > 0 distorts the premise that exchanger::push pushes a single element onto the lua stack because it removes nUpValues from the lua stack
                template<typename F>
                static void push(lua_State *luaState, F &&luaFunction, int nUpValues = 0);

            private:
                static const char * const kMetatableName_;
            };
        }

        //--

        inline int LuaFunctionWrapper::getUpValueIndex(int index) {
            return lua_upvalueindex(index + 1);
        }

        template<typename T>
        inline LuaFunctionWrapper::LuaFunctionWrapper(T &&luaFunction) : luaFunction_(std::forward<T>(luaFunction)) {}

        inline const std::function<int(lua_State *)> & LuaFunctionWrapper::getLuaFunction() const {
            return luaFunction_;
        }

        namespace exchanger {
            template<typename F>
            void Exchanger<LuaFunctionWrapper>::push(lua_State *luaState, F &&luaFunction, int nUpValues) {
                basic::pushUserData<LuaFunctionWrapper>(luaState, std::forward<F>(luaFunction));
                basic::pushClassMetatable<LuaFunctionWrapper>(luaState, kMetatableName_);
                lua_setmetatable(luaState, -2);
                if (nUpValues != 0) {
                    lua_insert(luaState, -1 - nUpValues);
                }
                lua_pushcclosure(luaState, [](lua_State *luaState) -> int {
                    try {
                        LuaFunctionWrapper *luaFunctionWrapper = static_cast<LuaFunctionWrapper *>(lua_compatibility::testudata(luaState, lua_upvalueindex(1), kMetatableName_));
                        if (luaFunctionWrapper != nullptr) {
                            return luaFunctionWrapper->getLuaFunction()(luaState);
                        } else {
                            throw exception::LogicException(__FILE__, __LINE__, __func__, "corrupted LuaFunctionWrapper");
                        }
                    } catch (const std::exception &exception) {
                        lua_pushstring(luaState, (std::string("[integral] ") + exception.what()).c_str());
                    } catch (...) {
                        lua_pushstring(luaState, message::gkUnknownExceptionMessage);
                    }
                    // error return outside catch scope so that the exception destructor can be called
                    return lua_error(luaState);
                }, 1 + nUpValues);
            }
        }
    }
}

#endif
