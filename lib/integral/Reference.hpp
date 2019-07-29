//
//  Reference.hpp
//  integral
//
//  Copyright (C) 2016, 2017  André Pereira Henriques
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

#ifndef integral_Reference_hpp
#define integral_Reference_hpp

#include <type_traits>
#include <utility>
#include <lua.hpp>
#include <exception/Exception.hpp>
#include <exception/TemplateClassException.hpp>
#include "ArgumentException.hpp"
#include "Caller.hpp"
#include "exchanger.hpp"
#include "factory.hpp"
#include "GlobalReference.hpp"
#include "serializer.hpp"

namespace integral {
    namespace detail {
        template<typename K, typename C>
        class Reference {
            static_assert(std::is_reference<K>::value == false, "K cannot be a reference type");
            static_assert(std::is_reference<C>::value == false, "C cannot be a reference type");
        public:
            // non-copyable
            Reference(const Reference &) = delete;
            Reference & operator=(const Reference &) = delete;

            Reference(Reference &&) = default;

            template<typename L, typename D>
            inline Reference(L &&key, D &&chainedReference);

            inline lua_State * getLuaState() const;
            void push() const;
            bool isNil() const;
            std::string getReferenceString() const;

            template<typename L>
            inline Reference<typename std::decay<L>::type, Reference<K, C>> operator[](L &&key) &&;

            template<typename T, typename ...A>
            Reference<K, C> & emplace(A &&...arguments);

            template<typename V>
            inline Reference<K, C> & set(V &&value);

            template<typename V>
            inline Reference<K, C> & operator=(V &&value);

            template<typename F, typename ...E, std::size_t ...I>
            inline Reference<K, C> & setFunction(F &&function, DefaultArgument<E, I> &&...defaultArguments);

            template<typename F>
            inline Reference<K, C> & setLuaFunction(F &&function);

            template<typename V>
            decltype(auto) get() const;

            // the conversion operator does not return a reference to an object
            // storing a reference to a Lua object is unsafe because it might be collected
            // Reference::get returns a reference to an object but it is not meant to be stored
            template<typename V>
            inline operator V() const;

            // the arguments are pushed by value onto the lua stack
            template<typename R, typename ...A>
            decltype(auto) call(A &&...arguments);

        private:
            K key_;
            C chainedReference_;
        };

        using ReferenceException = exception::TemplateClassException<Reference, exception::RuntimeException>;

        //--

        template<typename K, typename C>
        template<typename L, typename D>
        inline Reference<K, C>::Reference(L &&key, D &&chainedReference) : key_(std::forward<L>(key)), chainedReference_(std::forward<D>(chainedReference)) {}

        template<typename K, typename C>
        inline lua_State * Reference<K, C>::getLuaState() const {
            return chainedReference_.getLuaState();
        }

        template<typename K, typename C>
        inline void Reference<K, C>::push() const {
            chainedReference_.push();
            // stack: ?
            if (lua_istable(getLuaState(), -1) != 0) {
                // stack: chainedReferenceTable
                exchanger::push<K>(getLuaState(), key_);
                // stack: chainedReferenceTable | key
                lua_rawget(getLuaState(), -2);
                // stack: chainedReferenceTable | reference
                lua_remove(getLuaState(), -2);
                // stack: reference
            } else {
                // stack: ?
                lua_pop(getLuaState(), 1);
                throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] reference ") + chainedReference_.getReferenceString() + " is not a table");
            }
        }

        template<typename K, typename C>
        bool Reference<K, C>::isNil() const {
            push();
            // stack: ?
            if (lua_isnil(getLuaState(), -1) == 0) {
                // stack: ?
                lua_pop(getLuaState(), 1);
                return false;
            } else {
                // stack: nil
                lua_pop(getLuaState(), 1);
                return true;
            }
        }

        template<typename K, typename C>
        std::string Reference<K, C>::getReferenceString() const {
            return chainedReference_.getReferenceString() + "[" + serializer::getString(key_) + "]";
        }


        template<typename K, typename C>
        template<typename L>
        inline Reference<typename std::decay<L>::type, Reference<K, C>> Reference<K, C>::operator[](L &&key) && {
            return Reference<typename std::decay<L>::type, Reference<K, C>>(std::forward<L>(key), std::move(*this));
        }

        template<typename K, typename C>
        template<typename T, typename ...A>
        Reference<K, C> & Reference<K, C>::emplace(A &&...arguments) {
            chainedReference_.push();
            // stack: ?
            if (lua_istable(getLuaState(), -1) != 0) {
                // stack: chainedReferenceTable
                exchanger::push<K>(getLuaState(), key_);
                // stack: chainedReferenceTable | key
                exchanger::push<T>(getLuaState(), std::forward<A>(arguments)...);
                // stack: chainedReferenceTable | key | value
                lua_rawset(getLuaState(), -3);
                // stack: chainedReferenceTable
                lua_pop(getLuaState(), 1);
                // stack:
                return *this;
            } else {
                // stack: ?
                lua_pop(getLuaState(), 1);
                throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] reference ") + getReferenceString() + " is not a table");
            }
        }

        template<typename K, typename C>
        template<typename V>
        inline Reference<K, C> & Reference<K, C>::set(V &&value) {
            return emplace<typename std::decay<V>::type>(std::forward<V>(value));
        }

        template<typename K, typename C>
        template<typename V>
        inline Reference<K, C> & Reference<K, C>::operator=(V &&value) {
            return set(std::forward<V>(value));
        }

        template<typename K, typename C>
        template<typename F, typename ...E, std::size_t ...I>
        inline Reference<K, C> & Reference<K, C>::setFunction(F &&function, DefaultArgument<E, I> &&...defaultArguments) {
            return set(makeFunctionWrapper(std::forward<F>(function), std::move(defaultArguments)...));
        }

        template<typename K, typename C>
        template<typename F>
        inline Reference<K, C> & Reference<K, C>::setLuaFunction(F &&function) {
            return set(LuaFunctionWrapper(std::forward<F>(function)));
        }

        template<typename K, typename C>
        template<typename V>
        decltype(auto) Reference<K, C>::get() const {
            push();
            // stack: ?
            try {
                decltype(auto) returnValue = exchanger::get<V>(getLuaState(), -1);
                // stack: value
                lua_pop(getLuaState(), 1);
                // stack:
                return returnValue;
            } catch (const ArgumentException &argumentException) {
                // stack: ?
                lua_pop(getLuaState(), 1);
                // stack:
                throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] invalid type getting reference " ) + getReferenceString() + ": " + argumentException.what());
            }
        }
        
        template<typename K, typename C>
        template<typename V>
        inline Reference<K, C>::operator V() const {
            return get<V>();
        }

        template<typename K, typename C>
        template<typename R, typename ...A>
        decltype(auto) Reference<K, C>::call(A &&...arguments) {
            push();
            // stack: ?
            try {
                // detail::Caller<R, A...>::call pops the first element of the stack
                return detail::Caller<R, A...>::call(getLuaState(), std::forward<A>(arguments)...);
            } catch (const ArgumentException &argumentException) {
                throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] invalid type calling function reference " ) + getReferenceString() + ": " + argumentException.what());
            } catch (const CallerException &callerException) {
                throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] error calling function reference " ) + getReferenceString() + ": " + callerException.what());
            }
        }
    }
}

#endif
