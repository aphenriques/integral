//
//  ConstructorWrapper.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2015, 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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
