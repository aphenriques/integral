//
//  Reference.hpp
//  integral
//
//  Copyright (C) 2016, 2017, 2019  André Pereira Henriques
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

#include "ReferenceInterface.hpp"
#include "ArgumentException.hpp"
#include "Caller.hpp"

namespace integral {
    namespace detail {
        template<typename K, typename C>
        class Reference : public ReferenceInterface<Reference<K, C>> {
        public:
            template<typename L, typename D>
            inline Reference(L &&key, D &&chainedReference);

            bool isNil() const;

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
        };

        //--

        template<typename K, typename C>
        template<typename L, typename D>
        inline Reference<K, C>::Reference(L &&key, D &&chainedReference) :
            ReferenceInterface<Reference<K, C>>(std::forward<L>(key), std::forward<D>(chainedReference))
        {}

        template<typename K, typename C>
        bool Reference<K, C>::isNil() const {
            ReferenceInterface<Reference<K, C>>::push();
            // stack: ?
            if (lua_isnil((ReferenceInterface<Reference<K, C>>::getLuaState()), -1) == 0) {
                // stack: ?
                lua_pop((ReferenceInterface<Reference<K, C>>::getLuaState()), 1);
                return false;
            } else {
                // stack: nil
                lua_pop((ReferenceInterface<Reference<K, C>>::getLuaState()), 1);
                return true;
            }
        }

        template<typename K, typename C>
        template<typename T, typename ...A>
        Reference<K, C> & Reference<K, C>::emplace(A &&...arguments) {
            ReferenceInterface<Reference<K, C>>::getChainedReference().push();
            // stack: ?
            if (lua_istable((ReferenceInterface<Reference<K, C>>::getLuaState()), -1) != 0) {
                // stack: chainedReferenceTable
                exchanger::push<K>((ReferenceInterface<Reference<K, C>>::getLuaState()), ReferenceInterface<Reference<K, C>>::getKey());
                // stack: chainedReferenceTable | key
                exchanger::push<T>((ReferenceInterface<Reference<K, C>>::getLuaState()), std::forward<A>(arguments)...);
                // stack: chainedReferenceTable | key | value
                lua_rawset((ReferenceInterface<Reference<K, C>>::getLuaState()), -3);
                // stack: chainedReferenceTable
                lua_pop((ReferenceInterface<Reference<K, C>>::getLuaState()), 1);
                // stack:
                return *this;
            } else {
                // stack: ?
                lua_pop((ReferenceInterface<Reference<K, C>>::getLuaState()), 1);
                throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] reference ") + ReferenceInterface<Reference<K, C>>::getReferenceString() + " is not a table");
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
            ReferenceInterface<Reference<K, C>>::push();
            // stack: ?
            try {
                decltype(auto) returnValue = exchanger::get<V>((ReferenceInterface<Reference<K, C>>::getLuaState()), -1);
                // stack: value
                lua_pop((ReferenceInterface<Reference<K, C>>::getLuaState()), 1);
                // stack:
                return returnValue;
            } catch (const ArgumentException &argumentException) {
                // stack: ?
                lua_pop((ReferenceInterface<Reference<K, C>>::getLuaState()), 1);
                // stack:
                throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] invalid type getting reference " ) + ReferenceInterface<Reference<K, C>>::getReferenceString() + ": " + argumentException.what());
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
            ReferenceInterface<Reference<K, C>>::push();
            // stack: ?
            try {
                // detail::Caller<R, A...>::call pops the first element of the stack
                return detail::Caller<R, A...>::call((ReferenceInterface<Reference<K, C>>::getLuaState()), std::forward<A>(arguments)...);
            } catch (const ArgumentException &argumentException) {
                throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] invalid type calling function reference " ) + ReferenceInterface<Reference<K, C>>::getReferenceString() + ": " + argumentException.what());
            } catch (const CallerException &callerException) {
                throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] error calling function reference " ) + ReferenceInterface<Reference<K, C>>::getReferenceString() + ": " + callerException.what());
            }
        }
    }
}

#endif
