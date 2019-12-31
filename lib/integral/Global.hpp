//
//  Global.hpp
//  integral
//
//  Copyright (C) 2019  André Pereira Henriques
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

#ifndef integral_Global_hpp
#define integral_Global_hpp

#include "GlobalBase.hpp"
#include "ReferenceValue.hpp"

namespace integral {
    class Global : public GlobalBase {
    public:
        using GlobalBase::push;

        template<typename K>
        detail::ReferenceValue<std::decay_t<K>, Global> operator[](K &&key) &&;
    };

    namespace detail::exchanger {
        template<>
        class Exchanger<Global> {
        public:
            inline static void push(lua_State *luaState, const Global &global);
            inline static void push(lua_State *luaState);
        };
    }

    //--

    template<typename K>
    detail::ReferenceValue<std::decay_t<K>, Global> Global::operator[](K &&key) && {
        return {std::forward<K>(key), std::move(*this)};
    }

    namespace detail::exchanger {
        inline void Exchanger<Global>::push(lua_State *luaState, const Global &global) {
            global.push(luaState);
        }

        inline void Exchanger<Global>::push(lua_State *luaState) {
            Global().push(luaState);
        }
    }
}

#endif

