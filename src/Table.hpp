//
//  Table.hpp
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

#ifndef integral_Table_hpp
#define integral_Table_hpp

#include <type_traits>
#include <utility>
#include <lua.hpp>
#include "TableComposite.hpp"
#include "exchanger.hpp"

namespace integral {
    class Table {
    public:
        // non-copyable
        Table(const Table &) = delete;
        Table & operator=(const Table &) = delete;

        Table(Table &&) = default;
        Table() = default;

        template<typename K, typename V>
        inline detail::TableComposite<Table, typename std::decay<K>::type, typename std::decay<V>::type> set(K&& key, V &&value) &&;
    };

    namespace detail {
        namespace exchanger {
            template<>
            class Exchanger<Table> {
            public:
                inline static void push(lua_State *luaState);
                inline static void push(lua_State *luaState, const Table &);
            };
        }
    }

    //--

    template<typename K, typename V>
    inline detail::TableComposite<Table, typename std::decay<K>::type, typename std::decay<V>::type> Table::set(K&& key, V &&value) && {
        return detail::TableComposite<Table, typename std::decay<K>::type, typename std::decay<V>::type>(std::move(*this), std::forward<K>(key), std::forward<V>(value));
    }

    namespace detail {
        namespace exchanger {
            inline void Exchanger<Table>::push(lua_State *luaState) {
                lua_newtable(luaState);
            }

            inline void Exchanger<Table>::push(lua_State *luaState, const Table &) {
                push(luaState);
            }
        }
    }
}

#endif