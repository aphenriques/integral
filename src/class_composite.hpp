//
//  class_composite.hpp
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

#ifndef integral_class_composite_hpp
#define integral_class_composite_hpp

#include <type_traits>
#include <utility>
#include <lua.hpp>
#include "exchanger.hpp"
#include "type_manager.hpp"

namespace integral {
    namespace detail {
        namespace class_composite {
            // Forward declaration
            template<typename T, typename B, typename C>
            class InheritanceComposite;

            template<typename T, typename C, typename K, typename V>
            class ClassMetatableComposite;

            // T: class type
            // U: underlying type
            // curiously recurring template pattern (CRTP)
            template<typename T, typename U>
            class ClassCompositeInterface {
            public:
                template<typename L, typename W>
                inline ClassMetatableComposite<T, U, typename std::decay<L>::type, typename std::decay<W>::type> set(L &&key, W &&value) &&;

                // B: derived class type
                template<typename B>
                inline InheritanceComposite<T, B, U> setBaseClass() &&;

            };

            // T: Class type
            template<typename T, typename C, typename K, typename V>
            class ClassMetatableComposite : public ClassCompositeInterface<T, ClassMetatableComposite<T, C, K, V>> {
                static_assert(std::is_reference<C>::value == false, "C cannot be a reference type");
                static_assert(std::is_reference<K>::value == false, "K cannot be a reference type");
                static_assert(std::is_reference<V>::value == false, "V cannot be a reference type");
            public:
                // non-copyable
                ClassMetatableComposite(const ClassMetatableComposite &) = delete;
                ClassMetatableComposite & operator=(const ClassMetatableComposite &) = delete;

                ClassMetatableComposite(ClassMetatableComposite &&) = default;

                inline ClassMetatableComposite(C &&chainedClassMetatableComposite, K &&key, V &&value);

                void push(lua_State *luaState) const;

            private:
                C chainedClassMetatableComposite_;
                K key_;
                V value_;
            };

            // T: class type
            // B: base class type
            template<typename T, typename B, typename C>
            class InheritanceComposite : public ClassCompositeInterface<T, InheritanceComposite<T, B, C>> {
                static_assert(std::is_reference<C>::value == false, "C cannot be a reference type");
            public:
                // non-copyable
                InheritanceComposite(const InheritanceComposite &) = delete;
                InheritanceComposite & operator=(const InheritanceComposite &) = delete;

                InheritanceComposite(InheritanceComposite &&) = default;

                inline InheritanceComposite(C &&chainedClassMetatableComposite);

                void push(lua_State *luaState) const;

            private:
                C chainedClassMetatableComposite_;
            };
        }

        namespace exchanger {
            template<typename T, typename C, typename K, typename V>
            class Exchanger<class_composite::ClassMetatableComposite<T, C, K, V>> {
            public:
                inline static void push(lua_State *luaState, const class_composite::ClassMetatableComposite<T, C, K, V> &composite);
            };
        }

        namespace exchanger {
            template<typename T, typename B, typename C>
            class Exchanger<class_composite::InheritanceComposite<T, B, C>> {
            public:
                inline static void push(lua_State *luaState, const class_composite::InheritanceComposite<T, B, C> &composite);
            };
        }

            //--

        namespace class_composite {
            // ClassCompositeInterface
            template<typename T, typename U>
            template<typename L, typename W>
            inline ClassMetatableComposite<T, U, typename std::decay<L>::type, typename std::decay<W>::type> ClassCompositeInterface<T, U>::set(L &&key, W &&value) && {
                return ClassMetatableComposite<T, U, typename std::decay<L>::type, typename std::decay<W>::type>(std::move(*static_cast<U *>(this)), std::forward<L>(key), std::forward<W>(value));
            }

            template<typename T, typename U>
            template<typename B>
            inline InheritanceComposite<T, B, U> ClassCompositeInterface<T, U>::setBaseClass() && {
                return InheritanceComposite<T, B, U>(std::move(*static_cast<U *>(this)));
            }

            // ClassMetatableComposite
            template<typename T, typename C, typename K, typename V>
            inline ClassMetatableComposite<T, C, K, V>::ClassMetatableComposite(C &&chainedClassMetatableComposite, K &&key, V &&value) : chainedClassMetatableComposite_(std::forward<C>(chainedClassMetatableComposite)), key_(std::forward<K>(key)), value_(std::forward<V>(value)) {}

            template<typename T, typename C, typename K, typename V>
            void ClassMetatableComposite<T, C, K, V>::push(lua_State *luaState) const {
                exchanger::push<C>(luaState, chainedClassMetatableComposite_);
                // stack: table 
                exchanger::push<K>(luaState, key_);
                // stack: table | key
                exchanger::push<V>(luaState, value_);
                // stack: table | key | value
                lua_rawset(luaState, -3);
                // stack: table 
            }

            // InheritanceComposite
            template<typename T, typename B, typename C>
            inline InheritanceComposite<T, B, C>::InheritanceComposite(C &&chainedClassMetatableComposite) : chainedClassMetatableComposite_(std::forward<C>(chainedClassMetatableComposite)) {}

            template<typename T, typename B, typename C>
            void InheritanceComposite<T, B, C>::push(lua_State *luaState) const {
                exchanger::push<C>(luaState, chainedClassMetatableComposite_);
                // stack: table 
                type_manager::defineInheritance<T, B>(luaState);
                // stack: table 
            }
        }

        namespace exchanger {
            template<typename T, typename C, typename K, typename V>
            inline void Exchanger<class_composite::ClassMetatableComposite<T, C, K, V>>::push(lua_State *luaState, const class_composite::ClassMetatableComposite<T, C, K, V> &composite) {
                composite.push(luaState);
            }
        }

        namespace exchanger {
            template<typename T, typename B, typename C>
            inline void Exchanger<class_composite::InheritanceComposite<T, B, C>>::push(lua_State *luaState, const class_composite::InheritanceComposite<T, B, C> &composite) {
                composite.push(luaState);
            }
        }
    }
}

#endif