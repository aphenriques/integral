//
//  ReferenceInterface.hpp
//  integral
//
//  Copyright (C) 2019  André Pereira Henriques
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

#ifndef integral_ReferenceInterface_hpp
#define integral_ReferenceInterface_hpp

#include <type_traits>
#include <utility>
#include <lua.hpp>
#include <exception/Exception.hpp>
#include <exception/TemplateClassException.hpp>
#include "exchanger.hpp"
#include "serializer.hpp"

namespace integral::detail {
    template<typename T>
    class ReferenceInterface;

    template<template<typename, typename> typename T, typename K, typename C>
    class ReferenceInterface<T<K, C>> {
        static_assert(std::is_same_v<T<K, C>, std::decay_t<T<K, C>>> == true, "T<K, C> must not be either const or reference");
        static_assert(std::is_same_v<K, std::decay_t<K>> == true, "K must not be either const or reference");
        static_assert(std::is_same_v<C, std::decay_t<C>> == true, "C must not be either const or reference");
    public:
        // non-copyable
        ReferenceInterface(const ReferenceInterface &) = delete;
        ReferenceInterface & operator=(const ReferenceInterface &) = delete;

        ReferenceInterface(ReferenceInterface &&) = default;

        template<typename L, typename D>
        inline ReferenceInterface(L &&key, D &&chainedReference);

        template<typename L>
        inline T<typename std::decay<L>::type, T<K, C>> operator[](L &&key) &&;

        void push() const;
        std::string getReferenceString() const;
        inline lua_State * getLuaState() const;

    protected:
        ReferenceInterface() = default;

        inline const K & getKey() const;
        inline const C & getChainedReference() const;

    private:
        K key_;
        C chainedReference_;
    };

    using ReferenceException = exception::TemplateClassException<ReferenceInterface, exception::RuntimeException>;

    template<template<typename, typename> typename T, typename K, typename C>
    template<typename L, typename D>
    inline ReferenceInterface<T<K, C>>::ReferenceInterface(L &&key, D &&chainedReference) :
        key_(std::forward<L>(key)),
        chainedReference_(std::forward<D>(chainedReference))
    {}

    template<template<typename, typename> typename T, typename K, typename C>
    template<typename L>
    inline T<typename std::decay<L>::type, T<K, C>> ReferenceInterface<T<K, C>>::operator[](L &&key) && {
        return {std::forward<L>(key), std::move(static_cast<T<K, C> &>(*this))};
    }

    template<template<typename, typename> typename T, typename K, typename C>
    void ReferenceInterface<T<K, C>>::push() const {
        getChainedReference().push();
        // stack: ?
        if (lua_istable(getLuaState(), -1) != 0) {
            // stack: chainedReferenceTable
            exchanger::push<K>(getLuaState(), getKey());
            // stack: chainedReferenceTable | key
            lua_rawget(getLuaState(), -2);
            // stack: chainedReferenceTable | reference
            lua_remove(getLuaState(), -2);
            // stack: reference
        } else {
            // stack: ?
            lua_pop(getLuaState(), 1);
            throw ReferenceException(__FILE__, __LINE__, __func__, std::string("[integral] reference ") + getChainedReference().getReferenceString() + " is not a table");
        }
    }

    template<template<typename, typename> typename T, typename K, typename C>
    std::string ReferenceInterface<T<K, C>>::getReferenceString() const {
        return getChainedReference().getReferenceString() + "[" + serializer::getString(getKey()) + "]";
    }

    template<template<typename, typename> typename T, typename K, typename C>
    inline lua_State * ReferenceInterface<T<K, C>>::getLuaState() const {
        return getChainedReference().getLuaState();
    }

    template<template<typename, typename> typename T, typename K, typename C>
    inline const K & ReferenceInterface<T<K, C>>::getKey() const {
        return key_;
    }

    template<template<typename, typename> typename T, typename K, typename C>
    inline const C & ReferenceInterface<T<K, C>>::getChainedReference() const {
        return chainedReference_;
    }
}

#endif

