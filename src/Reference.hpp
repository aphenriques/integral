//
//  Reference.hpp
//  integral
//
//  Copyright (C) 2016  André Pereira Henriques
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

#include <utility>
#include <lua.hpp>
#include "exception/Exception.hpp"
#include "exception/TemplateClassException.hpp"
#include "Adaptor.hpp"
#include "ArgumentException.hpp"
#include "exchanger.hpp"
#include "generic.hpp"
#include "GlobalReference.hpp"
#include "IsTemplateClass.hpp"
#include "serializer.hpp"

namespace integral {
    namespace detail {
        template<typename K, typename R>
        class Reference {
        public:
            // non-copyable
            Reference(const Reference &) = delete;
            Reference<K, R> & operator=(const Reference &) = delete;

            Reference(Reference &&) = default;

            inline lua_State * getLuaState() const;
            void push() const;
            std::string getReferenceString() const;

            template<typename L>
            inline Reference<L, Reference<K, R>> operator[](L &&key) &&;

            template<typename V>
            Reference<K, R> & set(V &&value);

            template<typename V>
            V get() const;

        protected:
            inline Reference(K &&key, R &&chainedReference);

        private:
            K key_;
            R chainedReference_;
        };

        using ReferenceException = exception::TemplateClassException<Reference, exception::RuntimeException>;

        //--

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
                // BasicType<K> must be used especially due to string literal type
                exchanger::push<generic::BasicType<K>>(getLuaState(), key_);
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
        inline Reference<L, Reference<K, R>> Reference<K, R>::operator[](L &&key) && {
            // Adaptor<detail::Reference<...>> is utilized to access protected constructor of detail::Reference<...>
            return Adaptor<detail::Reference<L, Reference<K, R>>>(std::forward<L>(key), std::move(*this));
        }

        template<typename K, typename R>
        template<typename V>
        Reference<K, R> & Reference<K, R>::set(V &&value) {
            chainedReference_.push();
            // stack: ?
            if (lua_istable(getLuaState(), -1) != 0) {
                // stack: chainedReferenceTable
                // BasicType<K> must be used especially due to string literal type
                exchanger::push<generic::BasicType<K>>(getLuaState(), key_);
                // stack: chainedReferenceTable | key
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

        template<typename K, typename R>
        inline Reference<K, R>::Reference(K &&key, R &&chainedReference) : key_(std::forward<K>(key)),  chainedReference_(std::forward<R>(chainedReference)) {}
    }
}

#endif
