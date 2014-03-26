//
//  constructor.h
//  integral
//
//  Copyright (C) 2013, 2014  André Pereira Henriques
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

#ifndef integral_constructor_h
#define integral_constructor_h

#include <lua.hpp>
#include "TemplateSequence.h"
#include "TemplateSequenceGenerator.h"
#include "exchanger.h"

namespace integral {
    namespace detail {
        namespace constructor {
            template<typename T, typename ...A>
            inline int callConstructor(lua_State *luaState);
            
            template<typename T, typename ...A, unsigned ...S>
            void callConstructor(lua_State *luaState, TemplateSequence<S...>);
            
            //--
            
            template<typename T, typename ...A>
            inline int callConstructor(lua_State *luaState) {
                callConstructor<T, A...>(luaState, typename TemplateSequenceGenerator<sizeof...(A)>::TemplateSequenceType());
                return 1;
            }
            
            template<typename T, typename ...A, unsigned ...S>
            void callConstructor(lua_State *luaState, TemplateSequence<S...>) {
                exchanger::push<T>(luaState, exchanger::get<A>(luaState, S + 1)...);
            }
        }
    }
}


#endif
