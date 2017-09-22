//
//  FunctionWrapper.hpp
//  integral
//
//  Copyright (C) 2013, 2014, 2015, 2016, 2017  André Pereira Henriques
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
