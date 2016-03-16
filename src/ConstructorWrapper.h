//
//  ConstructorWrapper.h
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

#ifndef integral_ConstructorWrapper_h
#define integral_ConstructorWrapper_h

#include <functional>
#include <string>
#include <utility>
#include <lua.hpp>
#include "argument.h"
#include "ArgumentException.h"
#include "DefaultArgument.h"
#include "DefaultArgumentManager.h"
#include "exchanger.h"
#include "LuaFunctionWrapper.h"
#include "type_count.h"

namespace integral {
    namespace detail {
        template<typename M, typename T, typename ...A>
        class ConstructorWrapper {
        public:
            template<typename ...E, unsigned ...I>
            static void pushConstructor(lua_State *luaState, DefaultArgument<E, I> &&...defaultArguments);
            
            template<typename ...E, unsigned ...I>
            static void setConstructor(lua_State *luaState, const std::string &name, DefaultArgument<E, I> &&...defaultArguments);
            
            int operator()(lua_State *luaState) const;
            
        private:
            using ConstructorWrapperType = ConstructorWrapper<M, T, A...>;
            
            M defaultArgumentManager_;
            
            template<typename ...E, unsigned ...I>
            inline ConstructorWrapper(DefaultArgument<E, I> &&...defaultArguments);
            
            template<unsigned ...S>
            void call(lua_State *luaState, std::integer_sequence<unsigned, S...>) const;
        };
        
        //--
        
        template<typename M, typename T, typename ...A>
        template<typename ...E, unsigned ...I>
        void ConstructorWrapper<M, T, A...>::pushConstructor(lua_State *luaState, DefaultArgument<E, I> &&...defaultArguments) {
            argument::validateDefaultArguments<A...>(defaultArguments...);
            LuaFunctionWrapper::pushFunction(luaState, std::function<int(lua_State *)>(ConstructorWrapperType(std::move(defaultArguments)...)), 0);
        }
        
        template<typename M, typename T, typename ...A>
        template<typename ...E, unsigned ...I>
        void ConstructorWrapper<M, T, A...>::setConstructor(lua_State *luaState, const std::string &name, DefaultArgument<E, I> &&...defaultArguments) {
            argument::validateDefaultArguments<A...>(defaultArguments...);
            LuaFunctionWrapper::setFunction(luaState, name, std::function<int(lua_State *)>(ConstructorWrapperType(std::move(defaultArguments)...)), 0);
        }
        
        template<typename M, typename T, typename ...A>
        int ConstructorWrapper<M, T, A...>::operator()(lua_State *luaState) const {
            // replicate code of maximum number of parameters checking in FunctionWrapper<...>::setFunction
            const unsigned numberOfArgumentsOnStack = static_cast<unsigned>(lua_gettop(luaState));
            // type_count::getTypeCount provides the pack expanded count
            constexpr unsigned keLuaNumberOfArguments = type_count::getTypeCount<A...>();
            if (numberOfArgumentsOnStack <= keLuaNumberOfArguments) {
                defaultArgumentManager_.processDefaultArguments(luaState, keLuaNumberOfArguments, numberOfArgumentsOnStack);
                constexpr unsigned keCppNumberOfArguments = sizeof...(A);
                call(luaState, std::make_integer_sequence<unsigned, keCppNumberOfArguments>());
                return 1;
            } else {
                throw ArgumentException(luaState, keLuaNumberOfArguments, numberOfArgumentsOnStack);
            }
        }
        
        template<typename M, typename T, typename ...A>
        template<typename ...E, unsigned ...I>
        inline ConstructorWrapper<M, T, A...>::ConstructorWrapper(DefaultArgument<E, I> &&...defaultArguments) : defaultArgumentManager_(std::move(defaultArguments)...) {}
        
        template<typename M, typename T, typename ...A>
        template<unsigned ...S>
        void ConstructorWrapper<M, T, A...>::call(lua_State *luaState, std::integer_sequence<unsigned, S...>) const {
            exchanger::push<T>(luaState, exchanger::get<A>(luaState, S + 1)...);
        }
    }
}

#endif
