//
//  argument.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_argument_hpp
#define integral_argument_hpp

#include <cstddef>
#include <type_traits>
#include <utility>
#include "ArgumentTag.hpp"
#include "DefaultArgument.hpp"
#include "generic.hpp"
#include "MultipleInheritancePack.hpp"

namespace integral {
    namespace detail {
        namespace argument {
            template<typename ...T, std::size_t ...S>
            constexpr auto createArgumentTagPack(std::index_sequence<S...>);

            template<typename ...T, std::size_t ...I>
            inline void checkDefaultArgumentTypeAndIndex(const MultipleInheritancePack<ArgumentTag<T, I>...> &);

            template<typename E, typename ...T, std::size_t ...I>
            inline void checkDefaultArgumentTypeAndIndex(const MultipleInheritancePack<ArgumentTag<T, I>...> &);

            template<typename ...A, typename ...T, std::size_t ...I>
            inline void validateDefaultArguments(const DefaultArgument<T, I> &...defaultArguments);

            //--

            template<typename ...T, std::size_t ...S>
            constexpr auto createArgumentTagPack(std::index_sequence<S...>) {
                return MultipleInheritancePack<ArgumentTag<typename std::decay<T>::type, S + 1>...>();
            }

            template<typename ...T, std::size_t ...I>
            inline void checkDefaultArgumentTypeAndIndex(const MultipleInheritancePack<ArgumentTag<T, I>...> &) {}

            template<typename E, typename ...T, std::size_t ...I>
            inline void checkDefaultArgumentTypeAndIndex(const MultipleInheritancePack<ArgumentTag<T, I>...> &) {
                static_assert(std::is_base_of<typename E::ArgumentTag, MultipleInheritancePack<ArgumentTag<T, I>...>>::value == true, "invalid default argument");
            }

            template<typename ...A, typename ...T, std::size_t ...I>
            inline void validateDefaultArguments(const DefaultArgument<T, I> &... /*defaultArguments*/) {
                // comma operator in "(checkDefaultArgumentTypeAndIndex<E>(...), 0)" is used for function call expansion
                // check if each default argument type and index is valid
                generic::expandDummyTemplatePack((checkDefaultArgumentTypeAndIndex<DefaultArgument<T, I>>(createArgumentTagPack<A...>(std::index_sequence_for<A...>())), 0)...);
                // check if there is only one default argument for each index
                MultipleInheritancePack<typename DefaultArgument<T, I>::ArgumentTag...>();
            }
        }
    }
}

#endif
