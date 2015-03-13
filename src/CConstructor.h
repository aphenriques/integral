//
//  CConstructor.h
//  integral
//
//  Copyright (C) 2014  André Pereira Henriques
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

#ifndef integral_CConstructor_h
#define integral_CConstructor_h

#include <functional>
#include "exchanger.h"
#include "ConstructorWrapper.h"

namespace integral {
    namespace detail {
        template<typename T, typename ...A>
        class CConstructor {};
        
        namespace exchanger {
            template<typename T, typename ...A>
            class Exchanger<CConstructor<T, A...>> {
            public:
                inline static void push(lua_State *luaState, CConstructor<T, A...>);
                inline static void push(lua_State *luaState);
            };
        }
        
        //--
        
        namespace exchanger {
            template<typename T, typename ...A>
            inline void Exchanger<CConstructor<T, A...>>::push(lua_State *luaState, CConstructor<T, A...>) {
                ConstructorWrapper<DefaultArgumentManager<>, T, A...>::pushConstructor(luaState);
            }
            
            template<typename T, typename ...A>
            inline void Exchanger<CConstructor<T, A...>>::push(lua_State *luaState) {
                ConstructorWrapper<DefaultArgumentManager<>, T, A...>::pushConstructor(luaState);
            }
        }
    }
}

#endif
