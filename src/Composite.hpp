//
//  Composite.hpp
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

#ifndef integral_Composite_hpp
#define integral_Composite_hpp

#include <type_traits>
#include <utility>
#include <lua.hpp>
#include "exchanger.hpp"
#include "generic.hpp"
#include "type_manager.hpp"

namespace integral {
    namespace detail {
        template<typename T, typename K, typename V>
        class Composite {
            static_assert(std::is_reference<T>::value == false, "T cannot be a reference type");
            static_assert(std::is_reference<K>::value == false, "K cannot be a reference type");
            static_assert(std::is_reference<V>::value == false, "V cannot be a reference type");
        public:
            // non-copyable
            Composite(const Composite &) = delete;
            Composite & operator=(const Composite &) = delete;

            Composite(Composite &&) = default;

            inline Composite(T &&chainedComposite, K &&key, V &&value);

            // generic::BasicType<...> is used because ... might be a reference
            template<typename L, typename W>
            inline Composite<Composite<T, K, V>, generic::BasicType<L>, generic::BasicType<W>> set(L &&key, W &&value) &&;

            void push(lua_State *luaState) const;
 
        private:
            T chainedComposite_;
            K key_;
            V value_;
        };

        namespace exchanger {
            template<typename T, typename K, typename V>
            class Exchanger<Composite<T, K, V>> {
            public:
                inline static void push(lua_State *luaState, const Composite<T, K, V> &composite);
            };
        }

        //--

        template<typename T, typename K, typename V>
        inline Composite<T, K, V>::Composite(T &&chainedComposite, K &&key, V &&value) : chainedComposite_(std::forward<T>(chainedComposite)), key_(std::forward<K>(key)), value_(std::forward<V>(value)) {}

        template<typename T, typename K, typename V>
        template<typename L, typename W>
        inline Composite<Composite<T, K, V>, generic::BasicType<L>, generic::BasicType<W>> Composite<T, K, V>::set(L &&key, W &&value) && {
            return Composite<Composite<T, K, V>, generic::BasicType<L>, generic::BasicType<W>>(std::move(*this), std::forward<L>(key), std::forward<W>(value));
        }

        template<typename T, typename K, typename V>
        void Composite<T, K, V>::push(lua_State *luaState) const {
            exchanger::push<T>(luaState, chainedComposite_);
            exchanger::push<K>(luaState, key_);
            exchanger::push<V>(luaState, value_);
            lua_rawset(luaState, -3);
        }

        namespace exchanger {
            template<typename T, typename K, typename V>
            inline void Exchanger<Composite<T, K, V>>::push(lua_State *luaState, const Composite<T, K, V> &composite) {
                composite.push(luaState);
            }
        }
    }
}

#endif