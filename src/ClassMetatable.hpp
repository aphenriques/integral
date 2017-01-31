//
//  ClassMetatable.hpp
//  integral
//
//  Copyright (C) 2017  André Pereira Henriques
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

#ifndef integral_ClassMetatable_hpp
#define integral_ClassMetatable_hpp

#include <lua.hpp>
#include "exchanger.hpp"
#include "type_manager.hpp"

namespace integral {
    namespace detail {
        template<typename T>
        class ClassMetatable {
        public:
            // non-copyable
            ClassMetatable(const ClassMetatable &) = delete;
            ClassMetatable & operator=(const ClassMetatable &) = delete;

            ClassMetatable(ClassMetatable &&) = default;

        protected:
            ClassMetatable() = default;
        };

        namespace exchanger {
            template<typename T>
            class Exchanger<ClassMetatable<T>> {
            public:
                inline static void push(lua_State *luaState);
            };
        }

        //--

        namespace exchanger {
            template<typename T>
            inline void Exchanger<ClassMetatable<T>>::push(lua_State *luaState) {
                type_manager::pushClassMetatable<T>(luaState);
            }
        }
    }
}

#endif