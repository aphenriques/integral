//
//  FunctionWrapper.hpp
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

#ifndef integral_FunctionWrapper_hpp
#define integral_FunctionWrapper_hpp

#include <exception>
#include <functional>
#include <string>
#include <utility>
#include <lua.hpp>
#include "exception/Exception.hpp"
#include "argument.hpp"
#include "ArgumentException.hpp"
#include "basic.hpp"
#include "DefaultArgument.hpp"
#include "DefaultArgumentManager.hpp"
#include "exchanger.hpp"
#include "FunctionCaller.hpp"
#include "message.hpp"
#include "LuaFunctionWrapper.hpp"
#include "type_count.hpp"
#include "type_manager.hpp"
#include "UserDataWrapper.hpp"

namespace integral {
    namespace detail {
        template<typename T>
        class FunctionWrapper;

        template<typename R, typename ...A>
        class FunctionWrapper<R(A...)> {
        public:
            template<typename T>
            inline FunctionWrapper(T &&function);

            // necessary because of the template constructor
            FunctionWrapper(const FunctionWrapper &) = default;
            FunctionWrapper(FunctionWrapper &) = default;
            FunctionWrapper(FunctionWrapper &&) = default;

            inline const std::function<R(A...)> & getFunction() const;

        private:
            std::function<R(A...)> function_;
        };

        namespace exchanger {
            template<typename R, typename ...A>
            class Exchanger<FunctionWrapper<R(A...)>> {
            public:
                template<typename T, typename ...E, unsigned ...I>
                static void push(lua_State *luaState, T &&function, DefaultArgument<E, I> &&...defaultArguments);
            };
        }

        //--

        template<typename R, typename ...A>
        template<typename T>
        inline FunctionWrapper<R(A...)>::FunctionWrapper(T &&function) : function_(std::forward<T>(function)) {}

        template<typename R, typename ...A>
        inline const std::function<R(A...)> & FunctionWrapper<R(A...)>::getFunction() const {
            return function_;
        }

        namespace exchanger {
            template<typename R, typename ...A>
            template<typename T, typename ...E, unsigned ...I>
            void Exchanger<FunctionWrapper<R(A...)>>::push(lua_State *luaState, T &&function, DefaultArgument<E, I> &&...defaultArguments) {
                argument::validateDefaultArguments<A...>(defaultArguments...);
                exchanger::push<LuaFunctionWrapper>(luaState, [functionWrapper = FunctionWrapper<R(A...)>(std::forward<T>(function)), defaultArgumentManager = DefaultArgumentManager<DefaultArgument<E, I>...>(std::move(defaultArguments)...)](lua_State *luaState) -> int {
                    // replicate code of maximum number of parameters checking in ConstructorWrapper<T, A...>::operator()
                    const unsigned numberOfArgumentsOnStack = static_cast<unsigned>(lua_gettop(luaState));
                    // type_count::getTypeCount provides the pack expanded count
                    constexpr unsigned keLuaNumberOfArguments = type_count::getTypeCount<A...>();
                    if (numberOfArgumentsOnStack <= keLuaNumberOfArguments) {
                         defaultArgumentManager.processDefaultArguments(luaState, keLuaNumberOfArguments, numberOfArgumentsOnStack);
                        return static_cast<int>(FunctionCaller<R, A...>::call(luaState, functionWrapper.getFunction(), std::make_integer_sequence<unsigned, sizeof...(A)>()));
                    } else {
                        throw ArgumentException(luaState, keLuaNumberOfArguments, numberOfArgumentsOnStack);
                    }
                });
            }
        }
    }
}

#endif
