//
//  Reference.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2016, 2017, 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_Reference_hpp
#define integral_Reference_hpp

#include "ReferenceBase.hpp"
#include "ArgumentException.hpp"
#include "Caller.hpp"

namespace integral::detail {
    template<typename K, typename C>
    class Reference : public ReferenceBase<Reference<K, C>> {
    public:
        template<typename L, typename D>
        inline Reference(L &&key, D &&chainedReference);

        inline lua_State * getLuaState() const;
        inline void push() const;
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
        ReferenceBase<Reference<K, C>>(std::forward<L>(key), std::forward<D>(chainedReference))
    {}

    template<typename K, typename C>
    inline lua_State * Reference<K, C>::getLuaState() const {
        return ReferenceBase<Reference<K, C>>::getChainedReference().getLuaState();
    }

    template<typename K, typename C>
    inline void Reference<K, C>::push() const {
        ReferenceBase<Reference<K, C>>::getChainedReference().push();
        ReferenceBase<Reference<K, C>>::pushValueFromTable(getLuaState());
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
    template<typename T, typename ...A>
    Reference<K, C> & Reference<K, C>::emplace(A &&...arguments) {
        ReferenceBase<Reference<K, C>>::getChainedReference().push();
        // stack: ?
        if (lua_istable(getLuaState(), -1) != 0) {
            // stack: chainedReferenceTable
            exchanger::push<K>(getLuaState(), ReferenceBase<Reference<K, C>>::getKey());
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
            throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] reference ") + ReferenceBase<Reference<K, C>>::getReferenceString() + " is not a table");
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
            throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] invalid type getting reference " ) + ReferenceBase<Reference<K, C>>::getReferenceString() + ": " + argumentException.what());
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
            throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] invalid type calling function reference " ) + ReferenceBase<Reference<K, C>>::getReferenceString() + ": " + argumentException.what());
        } catch (const CallerException &callerException) {
            throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] error calling function reference " ) + ReferenceBase<Reference<K, C>>::getReferenceString() + ": " + callerException.what());
        }
    }
}

#endif
