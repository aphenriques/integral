//
//  ConstructorWrapper.hpp
//  integral
//
//  Copyright (C) 2014, 2015, 2016  André Pereira Henriques
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

#ifndef integral_ConstructorWrapper_hpp
#define integral_ConstructorWrapper_hpp

#include <functional>
#include <string>
#include <utility>
#include <lua.hpp>
#include "argument.hpp"
#include "ArgumentException.hpp"
#include "DefaultArgument.hpp"
#include "DefaultArgumentManager.hpp"
#include "exchanger.hpp"
#include "LuaFunctionWrapper.hpp"

namespace integral {
    namespace detail {
        template<typename T, typename ...A>
        class ConstructorWrapper {};

        namespace exchanger {
            template<typename T, typename ...A>
            class Exchanger<ConstructorWrapper<T, A...>> {
            public:
                template<typename ...E, unsigned ...I>
                static void push(lua_State *luaState, DefaultArgument<E, I> &&...defaultArguments);

                template<typename ...E, unsigned ...I>
                inline static void push(lua_State *luaState, const ConstructorWrapper<T, A...>, DefaultArgument<E, I> &&...defaultArguments);

            private:
                template<unsigned ...S>
                static void callConstructor(lua_State *luaState, std::integer_sequence<unsigned, S...>);
            };
        }

        //--

        namespace exchanger {
            template<typename T, typename ...A>
            template<typename ...E, unsigned ...I>
            void Exchanger<ConstructorWrapper<T, A...>>::push(lua_State *luaState, DefaultArgument<E, I> &&...defaultArguments) {
                argument::validateDefaultArguments<A...>(defaultArguments...);
                exchanger::push<LuaFunctionWrapper>(luaState, [defaultArgumentManager = DefaultArgumentManager<DefaultArgument<E, I>...>(std::move(defaultArguments)...)](lua_State *luaState) -> int {
                    // replicate code of maximum number of parameters checking in FunctionWrapper<R(A...)>::setFunction
                    const unsigned numberOfArgumentsOnStack = static_cast<unsigned>(lua_gettop(luaState));
                    constexpr unsigned keCppNumberOfArguments = sizeof...(A);
                    if (numberOfArgumentsOnStack <= keCppNumberOfArguments) {
                        defaultArgumentManager.processDefaultArguments(luaState, keCppNumberOfArguments, numberOfArgumentsOnStack);
                        callConstructor(luaState, std::make_integer_sequence<unsigned, keCppNumberOfArguments>());
                        return 1;
                    } else {
                        throw ArgumentException(luaState, keCppNumberOfArguments, numberOfArgumentsOnStack);
                    }
                });
            }

            template<typename T, typename ...A>
            template<typename ...E, unsigned ...I>
            void Exchanger<ConstructorWrapper<T, A...>>::push(lua_State *luaState, const ConstructorWrapper<T, A...>, DefaultArgument<E, I> &&...defaultArguments) {
                push(luaState, std::move(defaultArguments)...);
            }

            template<typename T, typename ...A>
            template<unsigned ...S>
            void Exchanger<ConstructorWrapper<T, A...>>::callConstructor(lua_State *luaState, std::integer_sequence<unsigned, S...>) {
                exchanger::push<T>(luaState, exchanger::get<A>(luaState, S + 1)...);
            }
        }
    }
}

#endif
