//
//  argument.h
//  integral
//
//  Copyright (C) 2014, 2016  André Pereira Henriques
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

#ifndef integral_argument_h
#define integral_argument_h

#include <type_traits>
#include <utility>
#include "ArgumentTag.h"
#include "DefaultArgument.h"
#include "generic.h"
#include "MultipleInheritancePack.h"

namespace integral {
    namespace detail {
        namespace argument {
            template<typename ...T, unsigned ...S>
            constexpr auto createArgumentTagPack(std::integer_sequence<unsigned, S...>);
            
            template<typename ...T, unsigned ...I>
            inline void checkDefaultArgumentTypeAndIndex(const MultipleInheritancePack<ArgumentTag<T, I>...> &);
            
            template<typename E, typename ...T, unsigned ...I>
            inline void checkDefaultArgumentTypeAndIndex(const MultipleInheritancePack<ArgumentTag<T, I>...> &);
            
            template<typename ...A, typename ...T, unsigned ...I>
            inline void validateDefaultArguments(const DefaultArgument<T, I> &...defaultArguments);
            
            //--
            
            template<typename ...T, unsigned ...S>
            constexpr auto createArgumentTagPack(std::integer_sequence<unsigned, S...>) {
                return MultipleInheritancePack<ArgumentTag<generic::BasicType<T>, S + 1>...>();
            }
            
            template<typename ...T, unsigned ...I>
            inline void checkDefaultArgumentTypeAndIndex(const MultipleInheritancePack<ArgumentTag<T, I>...> &) {}
            
            template<typename E, typename ...T, unsigned ...I>
            inline void checkDefaultArgumentTypeAndIndex(const MultipleInheritancePack<ArgumentTag<T, I>...> &) {
                static_assert(std::is_base_of<typename E::ArgumentTag, MultipleInheritancePack<ArgumentTag<T, I>...>>::value == true, "invalid default argument");
            }
            
            template<typename ...A, typename ...T, unsigned ...I>
            inline void validateDefaultArguments(const DefaultArgument<T, I> &...defaultArguments) {
                // comma operator in "(checkDefaultArgumentTypeAndIndex<E>(...), 0)" is used for function call expansion
                // check if each default argument type and index is valid
                generic::expandDummyTemplatePack((checkDefaultArgumentTypeAndIndex<DefaultArgument<T, I>>(createArgumentTagPack<A...>(std::make_integer_sequence<unsigned, sizeof...(A)>())), 0)...);
                // check if there is only one default argument for each index
                MultipleInheritancePack<typename DefaultArgument<T, I>::ArgumentTag...>();
            }
        }
    }
}

#endif
