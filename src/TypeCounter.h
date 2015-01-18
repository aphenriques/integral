//
//  TypeCounter.h
//  integral
//
//  Copyright (C) 2014  André Pereira Henriques
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


#ifndef integral_TypeCounter_h
#define integral_TypeCounter_h

#include "exchanger.h"

namespace integral {
    namespace detail {
        template<typename T>
        class TypeCounter {
        public:
            static constexpr unsigned getCount();
        };
        
        template<typename ...T>
        class TypeCounter<LuaPack<T...>> {
        public:
            static constexpr unsigned getCount();
        };
        
        //--
        
        template<typename T>
        constexpr unsigned TypeCounter<T>::getCount() {
            return 1;
        }
        
        template<typename ...T>
        constexpr unsigned TypeCounter<LuaPack<T...>>::getCount() {
            return sizeof...(T);
        }
    }
}

#endif
