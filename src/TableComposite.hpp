//
//  TableComposite.hpp
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

#ifndef integral_TableComposite_hpp
#define integral_TableComposite_hpp

#include <type_traits>
#include <utility>
#include <lua.hpp>
#include "exchanger.hpp"
#include "type_manager.hpp"

namespace integral {
    namespace detail {
        // Attention! the code from ClassMetatableComposite.hpp is mostly a replication from TableComposite.hpp. Do not change the contents of one file without considering the other
        template<typename C, typename K, typename V>
        class TableComposite {
            static_assert(std::is_reference<C>::value == false, "C cannot be a reference type");
            static_assert(std::is_reference<K>::value == false, "K cannot be a reference type");
            static_assert(std::is_reference<V>::value == false, "V cannot be a reference type");
        public:
            // non-copyable
            TableComposite(const TableComposite &) = delete;
            TableComposite & operator=(const TableComposite &) = delete;

            TableComposite(TableComposite &&) = default;

            inline TableComposite(C &&chainedTableComposite, K &&key, V &&value);

            template<typename L, typename W>
            inline TableComposite<TableComposite<C, K, V>, typename std::decay<L>::type, typename std::decay<W>::type> set(L &&key, W &&value) &&;

            void push(lua_State *luaState) const;
 
        private:
            C chainedTableComposite_;
            K key_;
            V value_;
        };

        namespace exchanger {
            template<typename C, typename K, typename V>
            class Exchanger<TableComposite<C, K, V>> {
            public:
                inline static void push(lua_State *luaState, const TableComposite<C, K, V> &composite);
            };
        }

        //--

        template<typename C, typename K, typename V>
        inline TableComposite<C, K, V>::TableComposite(C &&chainedTableComposite, K &&key, V &&value) : chainedTableComposite_(std::forward<C>(chainedTableComposite)), key_(std::forward<K>(key)), value_(std::forward<V>(value)) {}

        template<typename C, typename K, typename V>
        template<typename L, typename W>
        inline TableComposite<TableComposite<C, K, V>, typename std::decay<L>::type, typename std::decay<W>::type> TableComposite<C, K, V>::set(L &&key, W &&value) && {
            return TableComposite<TableComposite<C, K, V>, typename std::decay<L>::type, typename std::decay<W>::type>(std::move(*this), std::forward<L>(key), std::forward<W>(value));
        }

        template<typename C, typename K, typename V>
        void TableComposite<C, K, V>::push(lua_State *luaState) const {
            exchanger::push<C>(luaState, chainedTableComposite_);
            // stack: table 
            exchanger::push<K>(luaState, key_);
            // stack: table | key
            exchanger::push<V>(luaState, value_);
            // stack: table | key | value
            lua_rawset(luaState, -3);
            // stack: table 
        }

        namespace exchanger {
            template<typename C, typename K, typename V>
            inline void Exchanger<TableComposite<C, K, V>>::push(lua_State *luaState, const TableComposite<C, K, V> &composite) {
                composite.push(luaState);
            }
        }
    }
}

#endif
