//
//  CFunction.h
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

#ifndef integral_CFunction_h
#define integral_CFunction_h

#include <functional>
#include "Adaptor.h"
#include "exchanger.h"
#include "FunctionWrapper.h"

namespace integral {
    namespace detail {
        template<typename T>
        using CFunction = Adaptor<std::function<T>>;
        
        namespace exchanger {
            template<typename R, typename ...A>
            class Exchanger<CFunction<R(A...)>> {
            public:
                inline static void push(lua_State *luaState, const CFunction<R(A...)> &function);
            };
        }
        
        //--
        
        namespace exchanger {
            template<typename R, typename ...A>
            inline void Exchanger<CFunction<R(A...)>>::push(lua_State *luaState, const CFunction<R(A...)> &function) {
                FunctionWrapper<DefaultArgumentManager<>, R, A...>::pushFunction(luaState, function);
            }
        }
    }
}

#endif
