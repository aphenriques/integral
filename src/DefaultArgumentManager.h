//
//  DefaultArgumentManager.h
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

#ifndef integral_DefaultArgumentManager_h
#define integral_DefaultArgumentManager_h

#include <tuple>
#include <utility>
#include <lua.hpp>
#include "DefaultArgument.h"
#include "exchanger.h"
#include "generic.h"

namespace integral {
    namespace detail {
        template<typename ...F>
        class DefaultArgumentManager {
        public:
            template<typename ...E, unsigned ...I>
            inline DefaultArgumentManager(DefaultArgument<E, I> &&...defaultArguments);
            
            void processDefaultArguments(lua_State *luaState, const unsigned numberOfFunctionArguments, const unsigned numberOfArgumentsOnStack) const;
            
        private:            
            std::tuple<F...> defaultArguments_;
            
            template<unsigned ...S>
            inline void processDefaultArguments(lua_State *luaState, std::integer_sequence<unsigned, S...>) const;
            
            template<typename E, unsigned I>
            void processArgument(lua_State *luaState, const DefaultArgument<E, I> &defaultArgument) const;
        };
        
        //--
        
        template<typename ...F>
        template<typename ...E, unsigned ...I>
        inline DefaultArgumentManager<F...>::DefaultArgumentManager(DefaultArgument<E, I> &&...defaultArguments) : defaultArguments_(std::move(defaultArguments)...) {}
        
        template<typename ...F>
        void DefaultArgumentManager<F...>::processDefaultArguments(lua_State *luaState, const unsigned numberOfFunctionArguments, const unsigned numberOfArgumentsOnStack) const {
            if (numberOfArgumentsOnStack < numberOfFunctionArguments) {
                const unsigned numberOfArgumentsDifference = numberOfFunctionArguments - numberOfArgumentsOnStack;
                for (unsigned i = 0; i < numberOfArgumentsDifference; ++i) {
                    lua_pushnil(luaState);
                }
            }
            processDefaultArguments(luaState, std::make_integer_sequence<unsigned, sizeof...(F)>());
        }
        
        template<typename ...F>
        template<unsigned ...S>
        inline void DefaultArgumentManager<F...>::processDefaultArguments(lua_State *luaState, std::integer_sequence<unsigned, S...>) const {
            generic::expandDummyTemplatePack((processArgument(luaState, std::get<S>(defaultArguments_)), 0)...);
        }
        
        template<typename ...F>
        template<typename E, unsigned I>
        void DefaultArgumentManager<F...>::processArgument(lua_State *luaState, const DefaultArgument<E, I> &defaultArgument) const {
            if (lua_isnil(luaState, I) != 0) {
                exchanger::push<E>(luaState, defaultArgument.getArgument());
                lua_replace(luaState, I);
            }
        }
    }
}

#endif
