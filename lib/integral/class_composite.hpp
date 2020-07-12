//
//  class_composite.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2017, 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_class_composite_hpp
#define integral_class_composite_hpp

#include <functional>
#include <type_traits>
#include <utility>
#include <lua.hpp>
#include "ConversionFunctionTraits.hpp"
#include "Getter.hpp"
#include "Emplacer.hpp"
#include "exchanger.hpp"
#include "factory.hpp"
#include "FunctionTraits.hpp"
#include "Setter.hpp"
#include "type_manager.hpp"

namespace integral {
    namespace detail {
        namespace class_composite {
            // forward declarations
            template<typename T, typename B, typename C>
            class InheritanceComposite;

            template<typename T, typename C, typename K, typename V>
            class ClassMetatableComposite;

            template<typename T, typename C, typename F>
            class SyntheticInheritanceComposite;

            // T: class type
            // U: underlying type
            // curiously recurring template pattern (CRTP)
            template<typename T, typename U>
            class ClassCompositeInterface {
            public:
                template<typename K, typename V>
                inline ClassMetatableComposite<T, U, typename std::decay<K>::type, typename std::decay<V>::type> set(K &&key, V &&value) &&;

                template<typename W, typename K, typename ...A>
                inline ClassMetatableComposite<T, U, typename std::decay<K>::type, Emplacer<W, typename std::decay<A>::type...>> emplace(K &&key, A &&...arguments) &&;

                template<typename K, typename F, typename ...E, std::size_t ...I>
                inline decltype(auto) setFunction(K &&key, F &&function, DefaultArgument<E, I> &&...defaultArguments) &&;

                template<typename K, typename F>
                inline decltype(auto) setLuaFunction(K &&key, F &&luaFunction) &&;

                template<typename F, typename K, typename ...E, std::size_t ...I>
                inline decltype(auto) setConstructor(K &&key, DefaultArgument<E, I> &&...defaultArguments) &&;

                template<typename K, typename R, typename S>
                inline decltype(auto) setGetter(K &&key, R S::* attribute) &&;

                template<typename K, typename R, typename S>
                inline decltype(auto) setSetter(K &&key, R S::* attribute) &&;

                // B: base class type
                template<typename B>
                inline InheritanceComposite<T, B, U> setBaseClass() &&;

                // F: type function
                template<typename F>
                inline SyntheticInheritanceComposite<T, U, F> setBaseClass(F &&typeFunction) &&;
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

                template<typename L, typename W>
                inline ClassMetatableComposite(C &&chainedClassMetatableComposite, L &&key, W &&value);

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

            // T: class type
            // F: type function
            template<typename T, typename C, typename F>
            class SyntheticInheritanceComposite : public ClassCompositeInterface<T, SyntheticInheritanceComposite<T, C, F>> {
                static_assert(std::is_reference<C>::value == false, "C cannot be a reference type");
            public:
                // non-copyable
                SyntheticInheritanceComposite(const SyntheticInheritanceComposite &) = delete;
                SyntheticInheritanceComposite & operator=(const SyntheticInheritanceComposite &) = delete;

                SyntheticInheritanceComposite(SyntheticInheritanceComposite &&) = default;

                inline SyntheticInheritanceComposite(C &&chainedClassMetatableComposite, F &&typeFunction);

                void push(lua_State *luaState) const;

            private:
                C chainedClassMetatableComposite_;
                F typeFunction_;
            };
        }

        namespace exchanger {
            template<typename T, typename C, typename K, typename V>
            class Exchanger<class_composite::ClassMetatableComposite<T, C, K, V>> {
            public:
                inline static void push(lua_State *luaState, const class_composite::ClassMetatableComposite<T, C, K, V> &composite);
            };

            template<typename T, typename B, typename C>
            class Exchanger<class_composite::InheritanceComposite<T, B, C>> {
            public:
                inline static void push(lua_State *luaState, const class_composite::InheritanceComposite<T, B, C> &composite);
            };

            template<typename T, typename C, typename F>
            class Exchanger<class_composite::SyntheticInheritanceComposite<T, C, F>> {
            public:
                inline static void push(lua_State *luaState, const class_composite::SyntheticInheritanceComposite<T, C, F> &composite);
            };
        }

        //--

        namespace class_composite {
            // ClassCompositeInterface
            template<typename T, typename U>
            template<typename K, typename V>
            inline ClassMetatableComposite<T, U, typename std::decay<K>::type, typename std::decay<V>::type> ClassCompositeInterface<T, U>::set(K &&key, V &&value) && {
                return ClassMetatableComposite<T, U, typename std::decay<K>::type, typename std::decay<V>::type>(std::move(*static_cast<U *>(this)), std::forward<K>(key), std::forward<V>(value));
            }

            template<typename T, typename U>
            template<typename W, typename K, typename ...A>
            inline ClassMetatableComposite<T, U, typename std::decay<K>::type, Emplacer<W, typename std::decay<A>::type...>> ClassCompositeInterface<T, U>::emplace(K &&key, A &&...arguments) && {
                return ClassMetatableComposite<T, U, typename std::decay<K>::type, Emplacer<W, typename std::decay<A>::type...>>(std::move(*static_cast<U *>(this)), std::forward<K>(key), Emplacer<W, typename std::decay<A>::type...>(std::forward<A>(arguments)...));
            }

            template<typename T, typename U>
            template<typename K, typename F, typename ...E, std::size_t ...I>
            inline decltype(auto) ClassCompositeInterface<T, U>::setFunction(K &&key, F &&function, DefaultArgument<E, I> &&...defaultArguments) && {
                return std::move(*this).set(std::forward<K>(key), factory::makeFunctionWrapper(std::forward<F>(function), std::move(defaultArguments)...));
            }

            template<typename T, typename U>
            template<typename K, typename F>
            inline decltype(auto) ClassCompositeInterface<T, U>::setLuaFunction(K &&key, F &&luaFunction) && {
                return std::move(*this).set(std::forward<K>(key), LuaFunctionWrapper(std::forward<F>(luaFunction)));
            }

            template<typename T, typename U>
            template<typename F, typename K, typename ...E, std::size_t ...I>
            inline decltype(auto) ClassCompositeInterface<T, U>::setConstructor(K &&key, DefaultArgument<E, I> &&...defaultArguments) && {
                return std::move(*this).set(std::forward<K>(key), detail::factory::makeConstructorWrapper<F>(std::move(defaultArguments)...));
            }

            template<typename T, typename U>
            template<typename K, typename R, typename S>
            inline decltype(auto) ClassCompositeInterface<T, U>::setGetter(K &&key, R S::* attribute) && {
                return std::move(*this).setFunction(std::forward<K>(key), Getter<S, R>(attribute));
            }

            template<typename T, typename U>
            template<typename K, typename R, typename S>
            inline decltype(auto) ClassCompositeInterface<T, U>::setSetter(K &&key, R S::* attribute) && {
                return std::move(*this).setFunction(std::forward<K>(key), Setter<S, R>(attribute));
            }

            template<typename T, typename U>
            template<typename B>
            inline InheritanceComposite<T, B, U> ClassCompositeInterface<T, U>::setBaseClass() && {
                return InheritanceComposite<T, B, U>(std::move(*static_cast<U *>(this)));
            }

            template<typename T, typename U>
            template<typename F>
            inline SyntheticInheritanceComposite<T, U, F> ClassCompositeInterface<T, U>::setBaseClass(F &&typeFunction) && {
                using ConversionFunctionTraits = ConversionFunctionTraits<typename FunctionTraits<F>::Signature>;
                using OriginalType = typename ConversionFunctionTraits::OriginalType;
                // failing the following static_assert would NOT cause a bug. It exists to prevent an abstraction inconsistecy (it is a design choice)
                static_assert(std::is_same<OriginalType, T>::value == true, "type function original type mismatch with metatable class type");
                return SyntheticInheritanceComposite<T, U, F>(std::move(*static_cast<U *>(this)), std::forward<F>(typeFunction));
            }

            // ClassMetatableComposite
            template<typename T, typename C, typename K, typename V>
            template<typename L, typename W>
            inline ClassMetatableComposite<T, C, K, V>::ClassMetatableComposite(C &&chainedClassMetatableComposite, L &&key, W &&value) : chainedClassMetatableComposite_(std::forward<C>(chainedClassMetatableComposite)), key_(std::forward<L>(key)), value_(std::forward<W>(value)) {}

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

            // SyntheticInheritanceComposite
            template<typename T, typename C, typename F>
            inline SyntheticInheritanceComposite<T, C, F>::SyntheticInheritanceComposite(C &&chainedClassMetatableComposite, F &&typeFunction) : chainedClassMetatableComposite_(std::forward<C>(chainedClassMetatableComposite)), typeFunction_(std::forward<F>(typeFunction)) {}

            template<typename T, typename C, typename F>
            void SyntheticInheritanceComposite<T, C, F>::push(lua_State *luaState) const {
                exchanger::push<C>(luaState, chainedClassMetatableComposite_);
                // stack: table
                type_manager::defineInheritance(luaState, typeFunction_);
                // stack: table
            }
        }

        namespace exchanger {
            template<typename T, typename C, typename K, typename V>
            inline void Exchanger<class_composite::ClassMetatableComposite<T, C, K, V>>::push(lua_State *luaState, const class_composite::ClassMetatableComposite<T, C, K, V> &composite) {
                composite.push(luaState);
            }

            template<typename T, typename B, typename C>
            inline void Exchanger<class_composite::InheritanceComposite<T, B, C>>::push(lua_State *luaState, const class_composite::InheritanceComposite<T, B, C> &composite) {
                composite.push(luaState);
            }

            template<typename T, typename C, typename F>
            inline void Exchanger<class_composite::SyntheticInheritanceComposite<T, C, F>>::push(lua_State *luaState, const class_composite::SyntheticInheritanceComposite<T, C, F> &composite) {
                composite.push(luaState);
            }
        }
    }
}

#endif
