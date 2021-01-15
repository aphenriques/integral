//
//  ReferenceBase.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2019, 2020, 2021 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_ReferenceBase_hpp
#define integral_ReferenceBase_hpp

#include <type_traits>
#include <utility>
#include <lua.hpp>
#include <exception/Exception.hpp>
#include <exception/TemplateClassException.hpp>
#include "exchanger.hpp"
#include "serializer.hpp"

namespace integral::detail {
    template<typename T>
    class ReferenceBase;

    template<template<typename, typename> typename T, typename K, typename C>
    class ReferenceBase<T<K, C>> {
        static_assert(std::is_same_v<T<K, C>, std::decay_t<T<K, C>>> == true, "T<K, C> must not be either const or reference");
        static_assert(std::is_same_v<K, std::decay_t<K>> == true, "K must not be either const or reference");
        static_assert(std::is_same_v<C, std::decay_t<C>> == true, "C must not be either const or reference");

    public:
        // non-copyable
        ReferenceBase(const ReferenceBase &) = delete;
        ReferenceBase & operator=(const ReferenceBase &) = delete;

        ReferenceBase(ReferenceBase &&) = default;

        template<typename L, typename D>
        inline ReferenceBase(L &&key, D &&chainedReference);

        template<typename L>
        inline T<std::decay_t<L>, T<K, C>> operator[](L &&key) &&;

        std::string getReferenceString() const;

    protected:
        inline const K & getKey() const;
        inline const C & getChainedReference() const;

        // stack: table
        void pushValueFromTable(lua_State *luaState) const;

    private:
        K key_;
        C chainedReference_;
    };

    using ReferenceException = exception::TemplateClassException<ReferenceBase, exception::RuntimeException>;

    template<template<typename, typename> typename T, typename K, typename C>
    template<typename L, typename D>
    inline ReferenceBase<T<K, C>>::ReferenceBase(L &&key, D &&chainedReference) :
        key_(std::forward<L>(key)),
        chainedReference_(std::forward<D>(chainedReference))
    {}

    template<template<typename, typename> typename T, typename K, typename C>
    template<typename L>
    inline T<std::decay_t<L>, T<K, C>> ReferenceBase<T<K, C>>::operator[](L &&key) && {
        return {std::forward<L>(key), std::move(static_cast<T<K, C> &>(*this))};
    }

    template<template<typename, typename> typename T, typename K, typename C>
    std::string ReferenceBase<T<K, C>>::getReferenceString() const {
        return getChainedReference().getReferenceString() + "[" + serializer::getString(getKey()) + "]";
    }

    template<template<typename, typename> typename T, typename K, typename C>
    inline const K & ReferenceBase<T<K, C>>::getKey() const {
        return key_;
    }

    template<template<typename, typename> typename T, typename K, typename C>
    inline const C & ReferenceBase<T<K, C>>::getChainedReference() const {
        return chainedReference_;
    }

    template<template<typename, typename> typename T, typename K, typename C>
    void ReferenceBase<T<K, C>>::pushValueFromTable(lua_State *luaState) const {
        // stack: ?
        if (lua_istable(luaState, -1) != 0) {
            // stack: chainedReferenceTable
            exchanger::push<K>(luaState, getKey());
            // stack: chainedReferenceTable | key
            lua_rawget(luaState, -2);
            // stack: chainedReferenceTable | reference
            lua_remove(luaState, -2);
            // stack: reference
        } else {
            // stack: ?
            lua_pop(luaState, 1);
            throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] reference ") + getChainedReference().getReferenceString() + " is not a table");
        }
    }
}

#endif

