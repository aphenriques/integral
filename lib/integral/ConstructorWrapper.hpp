//
//  ConstructorWrapper.hpp
//  integral
//
//  Copyright (C) 2014, 2015, 2016, 2017  André Pereira Henriques
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

#include <cstddef>
#include <utility>
#include <lua.hpp>
#include "ArgumentException.hpp"
#include "DefaultArgument.hpp"
#include "DefaultArgumentManager.hpp"
#include "DefaultArgumentManagerContainer.hpp"
#include "exchanger.hpp"
#include "LuaFunctionWrapper.hpp"

namespace integral {
    namespace detail {
        template<typename T, typename M>
        class ConstructorWrapper;

        // "typename M" must be a DefaultArgumentManager<DefaultArgument<E, I>...>
        template<typename T, typename ...A, typename M>
        class ConstructorWrapper<T(A...), M> : public DefaultArgumentManagerContainer<M, A...> {
        public:
            // reuse DefaultArgumentManagerContainer constructor
            using DefaultArgumentManagerContainer<M, A...>::DefaultArgumentManagerContainer;
        };

        namespace exchanger {
            template<typename T, typename ...A, typename M>
            class Exchanger<ConstructorWrapper<T(A...), M>> {
            public:
                template<typename ...E, std::size_t ...I>
                inline static void push(lua_State *luaState, DefaultArgument<E, I> &&...defaultArguments);

                inline static void push(lua_State *luaState, ConstructorWrapper<T(A...), M> &&constructorWrapper);
                inline static void push(lua_State *luaState, const ConstructorWrapper<T(A...), M> &constructorWrapper);

            private:
                template<typename W>
                static void genericPush(lua_State *luaState, W &&constructorWrapper);

                template<std::size_t ...S>
                static void callConstructor(lua_State *luaState, std::index_sequence<S...>);
            };
        }

        //--

        namespace exchanger {
            template<typename T, typename ...A, typename M>
            template<typename ...E, std::size_t ...I>
            inline void Exchanger<ConstructorWrapper<T(A...), M>>::push(lua_State *luaState, DefaultArgument<E, I> &&...defaultArguments) {
                genericPush(luaState, ConstructorWrapper<T(A...), M>(std::move(defaultArguments)...));
            }

            template<typename T, typename ...A, typename M>
            inline void Exchanger<ConstructorWrapper<T(A...), M>>::push(lua_State *luaState, ConstructorWrapper<T(A...), M> &&constructorWrapper) {
                genericPush(luaState, std::move(constructorWrapper));
            }

            template<typename T, typename ...A, typename M>
            inline void Exchanger<ConstructorWrapper<T(A...), M>>::push(lua_State *luaState, const ConstructorWrapper<T(A...), M> &constructorWrapper) {
                genericPush(luaState, constructorWrapper);
            }

            template<typename T, typename ...A, typename M>
            template<typename W>
            inline void Exchanger<ConstructorWrapper<T(A...), M>>::genericPush(lua_State *luaState, W &&constructorWrapper) {
                exchanger::push<LuaFunctionWrapper>(luaState, [lambdaConstructorWrapper = std::forward<W>(constructorWrapper)](lua_State *lambdaLuaState) -> int {
                    // replicate code of maximum number of parameters checking in Exchanger<FunctionWrapper<R(A...), M>>::push
                    const std::size_t numberOfArgumentsOnStack = static_cast<std::size_t>(lua_gettop(lambdaLuaState));
                    constexpr std::size_t keCppNumberOfArguments = sizeof...(A);
                    if (numberOfArgumentsOnStack <= keCppNumberOfArguments) {
                        lambdaConstructorWrapper.getDefaultArgumentManager().processDefaultArguments(lambdaLuaState, keCppNumberOfArguments, numberOfArgumentsOnStack);
                        callConstructor(lambdaLuaState, std::make_index_sequence<keCppNumberOfArguments>());
                        return 1;
                    } else {
                        throw ArgumentException(lambdaLuaState, keCppNumberOfArguments, numberOfArgumentsOnStack);
                    }
                });
            }

            template<typename T, typename ...A, typename M>
            template<std::size_t ...S>
            void Exchanger<ConstructorWrapper<T(A...), M>>::callConstructor(lua_State *luaState, std::index_sequence<S...>) {
                exchanger::push<T>(luaState, exchanger::get<A>(luaState, S + 1)...);
            }
        }
    }
}

#endif
