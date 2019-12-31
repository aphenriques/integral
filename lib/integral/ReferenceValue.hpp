//
//  ReferenceValue.hpp
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

