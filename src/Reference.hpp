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
#include "exception/Exception.hpp"
#include "exception/TemplateClassException.hpp"
#include "ArgumentException.hpp"
#include "exchanger.hpp"
#include "generic.hpp"
#include "GlobalReference.hpp"
#include "serializer.hpp"

namespace integral {
    namespace detail {
        template<typename K, typename R>
        class Reference {
            static_assert(std::is_reference<K>::value == false, "K cannot be a reference type");
            static_assert(std::is_reference<R>::value == false, "R cannot be a reference type");
        public:
            // non-copyable
            Reference(const Reference &) = delete;
            Reference & operator=(const Reference &) = delete;

            Reference(Reference &&) = default;

            inline Reference(K &&key, R &&chainedReference);

            inline lua_State * getLuaState() const;
            void push() const;
            std::string getReferenceString() const;

            // generic::BasicType<L> is used because L might be a reference
            template<typename L>
            inline Reference<generic::BasicType<L>, Reference<K, R>> operator[](L &&key) &&;

            template<typename V>
            Reference<K, R> & set(V &&value);

            template<typename V>
            V get() const;

        private:
            K key_;
            R chainedReference_;
        };

        using ReferenceException = exception::TemplateClassException<Reference, exception::RuntimeException>;

        //--

        template<typename K, typename R>
        inline Reference<K, R>::Reference(K &&key, R &&chainedReference) : key_(std::forward<K>(key)), chainedReference_(std::forward<R>(chainedReference)) {}

        template<typename K, typename R>
        inline lua_State * Reference<K, R>::getLuaState() const {
            return chainedReference_.getLuaState();
        }

        template<typename K, typename R>
        inline void Reference<K, R>::push() const {
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

        template<typename K, typename R>
        std::string Reference<K, R>::getReferenceString() const {
            return chainedReference_.getReferenceString() + "[" + serializer::getString(key_) + "]";
        }


        template<typename K, typename R>
        template<typename L>
        inline Reference<generic::BasicType<L>, Reference<K, R>> Reference<K, R>::operator[](L &&key) && {
            return Reference<generic::BasicType<L>, Reference<K, R>>(std::forward<L>(key), std::move(*this));
        }

        template<typename K, typename R>
        template<typename V>
        Reference<K, R> & Reference<K, R>::set(V &&value) {
            chainedReference_.push();
            // stack: ?
            if (lua_istable(getLuaState(), -1) != 0) {
                // stack: chainedReferenceTable
                exchanger::push<K>(getLuaState(), key_);
                // stack: chainedReferenceTable | key
                // BasicType<V> is used because V might be a reference
                exchanger::push<generic::BasicType<V>>(getLuaState(), std::forward<V>(value));
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

        template<typename K, typename R>
        template<typename V>
        V Reference<K, R>::get() const {
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
                throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] invalid type getting reference  " ) + getReferenceString() + ": " + argumentException.what());
            }
        }
    }
}

#endif
