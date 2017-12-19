//
//  table_composite.hpp
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

#ifndef integral_table_composite_hpp
#define integral_table_composite_hpp

#include <type_traits>
#include <utility>
#include <lua.hpp>
#include "Emplacer.hpp"
#include "exchanger.hpp"
#include "factory.hpp"
#include "type_manager.hpp"

namespace integral {
    namespace detail {
        namespace table_composite {
            // forward declarations
            template<typename C, typename K, typename V>
            class TableComposite;

            // curiously recurring template pattern (CRTP)
            template<typename U>
            class TableCompositeInterface {
            public:
                template<typename K, typename V>
                inline TableComposite<U, typename std::decay<K>::type, typename std::decay<V>::type> set(K &&key, V &&value) &&;

                template<typename W, typename K, typename ...A>
                inline TableComposite<U, typename std::decay<K>::type, Emplacer<W, typename std::decay<A>::type...>> emplace(K &&key, A &&...arguments) &&;

                template<typename K, typename F, typename ...E, std::size_t ...I>
                inline decltype(auto) setFunction(K &&key, F &&function, DefaultArgument<E, I> &&...defaultArguments) &&;

                template<typename K, typename F>
                inline decltype(auto) setLuaFunction(K &&key, F &&luaFunction) &&;
            };

            template<typename C, typename K, typename V>
            class TableComposite : public TableCompositeInterface<TableComposite<C, K, V>> {
                static_assert(std::is_reference<C>::value == false, "C cannot be a reference type");
                static_assert(std::is_reference<K>::value == false, "K cannot be a reference type");
                static_assert(std::is_reference<V>::value == false, "V cannot be a reference type");
            public:
                // non-copyable
                TableComposite(const TableComposite &) = delete;
                TableComposite & operator=(const TableComposite &) = delete;

                TableComposite(TableComposite &&) = default;

                inline TableComposite(C &&chainedTableComposite, K &&key, V &&value);

                void push(lua_State *luaState) const;

            private:
                C chainedTableComposite_;
                K key_;
                V value_;
            };
        }

        namespace exchanger {
            template<typename C, typename K, typename V>
            class Exchanger<table_composite::TableComposite<C, K, V>> {
            public:
                inline static void push(lua_State *luaState, const table_composite::TableComposite<C, K, V> &composite);
            };
        }

        //--

        namespace table_composite {
            // TableCompositeInterface
            template<typename U>
            template<typename K, typename V>
            inline TableComposite<U, typename std::decay<K>::type, typename std::decay<V>::type> TableCompositeInterface<U>::set(K &&key, V &&value) && {
                return TableComposite<U, typename std::decay<K>::type, typename std::decay<V>::type>(std::move(*static_cast<U *>(this)), std::forward<K>(key), std::forward<V>(value));
            }

            template<typename U>
            template<typename W, typename K, typename ...A>
            inline TableComposite<U, typename std::decay<K>::type, Emplacer<W, typename std::decay<A>::type...>> TableCompositeInterface<U>::emplace(K &&key, A &&...arguments) && {
                return TableComposite<U, typename std::decay<K>::type, Emplacer<W, typename std::decay<A>::type...>>(std::move(*static_cast<U *>(this)), std::forward<K>(key), Emplacer<W, typename std::decay<A>::type...>(std::forward<A>(arguments)...));
            }

            template<typename U>
            template<typename K, typename F>
            inline decltype(auto) TableCompositeInterface<U>::setLuaFunction(K &&key, F &&luaFunction) && {
                return std::move(*this).set(std::forward<K>(key), LuaFunctionWrapper(std::forward<F>(luaFunction)));
            }

            template<typename U>
            template<typename K, typename F, typename ...E, std::size_t ...I>
            inline decltype(auto) TableCompositeInterface<U>::setFunction(K &&key, F &&function, DefaultArgument<E, I> &&...defaultArguments) && {
                return std::move(*this).set(std::forward<K>(key), makeFunctionWrapper(std::forward<F>(function), std::move(defaultArguments)...));
            }

            // TableComposite
            template<typename C, typename K, typename V>
            inline TableComposite<C, K, V>::TableComposite(C &&chainedTableComposite, K &&key, V &&value) : chainedTableComposite_(std::forward<C>(chainedTableComposite)), key_(std::forward<K>(key)), value_(std::forward<V>(value)) {}

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
        }

        namespace exchanger {
            template<typename C, typename K, typename V>
            inline void Exchanger<table_composite::TableComposite<C, K, V>>::push(lua_State *luaState, const table_composite::TableComposite<C, K, V> &composite) {
                composite.push(luaState);
            }
        }
    }
}

#endif
