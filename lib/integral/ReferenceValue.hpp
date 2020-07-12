//
//  ReferenceValue.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_ReferenceValue_hpp
#define integral_ReferenceValue_hpp

#include "ReferenceBase.hpp"

namespace integral::detail {
    template<typename K, typename C>
    class ReferenceValue : public ReferenceBase<ReferenceValue<K, C>> {
    public:
        template<typename L, typename D>
        inline ReferenceValue(L &&key, D &&chainedReference);

        inline void push(lua_State *luaState) const;
    };

    namespace exchanger {
        template<typename K, typename C>
        class Exchanger<ReferenceValue<K, C>> {
        public:
            inline static void push(lua_State *luaState, const ReferenceValue<K, C> &referenceValue);
        };
    }

    //--

    template<typename K, typename C>
    template<typename L, typename D>
    inline ReferenceValue<K, C>::ReferenceValue(L &&key, D &&chainedReference) :
        ReferenceBase<ReferenceValue<K, C>>(std::forward<L>(key), std::forward<D>(chainedReference))
    {}

    template<typename K, typename C>
    inline void ReferenceValue<K, C>::push(lua_State *luaState) const {
        ReferenceBase<ReferenceValue<K, C>>::getChainedReference().push(luaState);
        ReferenceBase<ReferenceValue<K, C>>::pushValueFromTable(luaState);
    }

    namespace exchanger {
        template<typename K, typename C>
        inline void Exchanger<ReferenceValue<K, C>>::push(lua_State *luaState, const ReferenceValue<K, C> &referenceValue) {
            referenceValue.push(luaState);
        }
    }
}

#endif

