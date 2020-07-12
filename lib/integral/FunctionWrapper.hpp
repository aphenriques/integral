//
//  FunctionWrapper.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2013, 2014, 2015, 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_FunctionWrapper_hpp
#define integral_FunctionWrapper_hpp

#include <cstddef>
#include <functional>
#include <utility>
#include <lua.hpp>
#include "ArgumentException.hpp"
#include "DefaultArgument.hpp"
#include "DefaultArgumentManager.hpp"
#include "DefaultArgumentManagerContainer.hpp"
#include "exchanger.hpp"
#include "FunctionCaller.hpp"
#include "LuaFunctionWrapper.hpp"

namespace integral {
    namespace detail {
        template<typename T, typename M>
        class FunctionWrapper;

        // "typename M" must be a DefaultArgumentManager<DefaultArgument<E, I>...>
        template<typename R, typename ...A, typename M>
        class FunctionWrapper<R(A...), M> : public DefaultArgumentManagerContainer<M, A...> {
        public:
            template<typename F, typename ...E, std::size_t ...I>
            inline FunctionWrapper(F &&function, DefaultArgument<E, I> &&...defaultArguments);

            // necessary because of the template constructor
            FunctionWrapper(const FunctionWrapper &) = default;
            FunctionWrapper(FunctionWrapper &) = default;
            FunctionWrapper(FunctionWrapper &&) = default;

            inline const std::function<R(A...)> & getFunction() const;

        private:
            std::function<R(A...)> function_;
        };

        namespace exchanger {
            template<typename R, typename ...A, typename M>
            class Exchanger<FunctionWrapper<R(A...), M>> {
            public:
                template<typename F, typename ...E, std::size_t ...I>
                inline static void push(lua_State *luaState, F &&function, DefaultArgument<E, I> &&...defaultArguments);

                inline static void push(lua_State *luaState, FunctionWrapper<R(A...), M> &&functionWrapper);
                inline static void push(lua_State *luaState, const FunctionWrapper<R(A...), M> &functionWrapper);

            private:
                template<typename W>
                static void genericPush(lua_State *luaState, W &&functionWrapper);
            };
        }

        //--

        template<typename R, typename ...A, typename M>
        template<typename F, typename ...E, std::size_t ...I>
        inline FunctionWrapper<R(A...), M>::FunctionWrapper(F &&function, DefaultArgument<E, I> &&...defaultArguments) : DefaultArgumentManagerContainer<M, A...>(std::move(defaultArguments)...), function_(std::forward<F>(function)) {}

        template<typename R, typename ...A, typename M>
        inline const std::function<R(A...)> & FunctionWrapper<R(A...), M>::getFunction() const {
            return function_;
        }

        namespace exchanger {
            template<typename R, typename ...A, typename M>
            template<typename F, typename ...E, std::size_t ...I>
            inline void Exchanger<FunctionWrapper<R(A...), M>>::push(lua_State *luaState, F &&function, DefaultArgument<E, I> &&...defaultArguments) {
                genericPush(luaState, FunctionWrapper<R(A...), M>(std::forward<F>(function), std::move(defaultArguments)...));
            }

            template<typename R, typename ...A, typename M>
            inline void Exchanger<FunctionWrapper<R(A...), M>>::push(lua_State *luaState, FunctionWrapper<R(A...), M> &&functionWrapper) {
                genericPush(luaState, std::move(functionWrapper));
            }

            template<typename R, typename ...A, typename M>
            inline void Exchanger<FunctionWrapper<R(A...), M>>::push(lua_State *luaState, const FunctionWrapper<R(A...), M> &functionWrapper) {
                genericPush(luaState, functionWrapper);
            }

            template<typename R, typename ...A, typename M>
            template<typename W>
            void Exchanger<FunctionWrapper<R(A...), M>>::genericPush(lua_State *luaState, W &&functionWrapper) {
                exchanger::push<LuaFunctionWrapper>(luaState, [lambdaFunctionWrapper = std::forward<W>(functionWrapper)](lua_State *lambdaLuaState) -> int {
                    // replicate code of maximum number of parameters checking in Exchanger<ConstructorWrapper<T(A...), M>>::push
                    const std::size_t numberOfArgumentsOnStack = static_cast<std::size_t>(lua_gettop(lambdaLuaState));
                    constexpr std::size_t keCppNumberOfArguments = sizeof...(A);
                    if (numberOfArgumentsOnStack <= keCppNumberOfArguments) {
                         lambdaFunctionWrapper.getDefaultArgumentManager().processDefaultArguments(lambdaLuaState, keCppNumberOfArguments, numberOfArgumentsOnStack);
                        return FunctionCaller<R, A...>::call(lambdaLuaState, lambdaFunctionWrapper.getFunction(), std::make_index_sequence<keCppNumberOfArguments>());
                    } else {
                        throw ArgumentException(lambdaLuaState, keCppNumberOfArguments, numberOfArgumentsOnStack);
                    }
                });
            }
        }
    }
}

#endif
