//
//  table_composite.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2017, 2019, 2020, 2021 André Pereira Henriques (aphenriques (at) outlook (dot) com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
                inline TableComposite<U, std::decay_t<K>, std::decay_t<V>> set(K &&key, V &&value) &&;

                template<typename W, typename K, typename ...A>
                inline TableComposite<U, std::decay_t<K>, Emplacer<W, std::decay_t<A>...>> emplace(K &&key, A &&...arguments) &&;

                template<typename K, typename F, typename ...E, std::size_t ...I>
                inline decltype(auto) setFunction(K &&key, F &&function, DefaultArgument<E, I> &&...defaultArguments) &&;

                template<typename K, typename F>
                inline decltype(auto) setLuaFunction(K &&key, F &&luaFunction) &&;
            };

            template<typename C, typename K, typename V>
            class TableComposite : public TableCompositeInterface<TableComposite<C, K, V>> {
                static_assert(std::is_reference_v<C> == false, "C cannot be a reference type");
                static_assert(std::is_reference_v<K> == false, "K cannot be a reference type");
                static_assert(std::is_reference_v<V> == false, "V cannot be a reference type");
            public:
                // non-copyable
                TableComposite(const TableComposite &) = delete;
                TableComposite & operator=(const TableComposite &) = delete;

                TableComposite(TableComposite &&) = default;

                template<typename L, typename W>
                inline TableComposite(C &&chainedTableComposite, L &&key, W &&value);

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
            inline TableComposite<U, std::decay_t<K>, std::decay_t<V>> TableCompositeInterface<U>::set(K &&key, V &&value) && {
                return TableComposite<U, std::decay_t<K>, std::decay_t<V>>(std::move(*static_cast<U *>(this)), std::forward<K>(key), std::forward<V>(value));
            }

            template<typename U>
            template<typename W, typename K, typename ...A>
            inline TableComposite<U, std::decay_t<K>, Emplacer<W, std::decay_t<A>...>> TableCompositeInterface<U>::emplace(K &&key, A &&...arguments) && {
                return TableComposite<U, std::decay_t<K>, Emplacer<W, std::decay_t<A>...>>(std::move(*static_cast<U *>(this)), std::forward<K>(key), Emplacer<W, std::decay_t<A>...>(std::forward<A>(arguments)...));
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
            template<typename L, typename W>
            inline TableComposite<C, K, V>::TableComposite(C &&chainedTableComposite, L &&key, W &&value) : chainedTableComposite_(std::forward<C>(chainedTableComposite)), key_(std::forward<L>(key)), value_(std::forward<W>(value)) {}

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
