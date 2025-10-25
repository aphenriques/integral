//
//  factory.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2017, 2020, 2024 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_factory_hpp
#define integral_factory_hpp

#include <utility>
#include "ConstructorWrapper.hpp"
#include "DefaultArgument.hpp"
#include "DefaultArgumentManager.hpp"
#include "FunctionTraits.hpp"
#include "FunctionWrapper.hpp"

namespace integral {
    namespace detail {
        namespace factory {
            template<typename F, typename ...E, std::size_t ...I>
            inline FunctionWrapper<typename detail::FunctionTraits<F>::Signature, detail::DefaultArgumentManager<DefaultArgument<E, I>...>> makeFunctionWrapper(F &&function, DefaultArgument<E, I> &&...defaultArguments);

            template<typename F, typename ...E, std::size_t ...I>
            inline ConstructorWrapper<F, detail::DefaultArgumentManager<DefaultArgument<E, I>...>> makeConstructorWrapper(DefaultArgument<E, I> &&...defaultArguments);

            //--

            template<typename F, typename ...E, std::size_t ...I>
            inline FunctionWrapper<typename detail::FunctionTraits<F>::Signature, detail::DefaultArgumentManager<DefaultArgument<E, I>...>> makeFunctionWrapper(F &&function, DefaultArgument<E, I> &&...defaultArguments) {
                return FunctionWrapper<typename detail::FunctionTraits<F>::Signature, detail::DefaultArgumentManager<DefaultArgument<E, I>...>>(std::forward<F>(function), std::move(defaultArguments)...);
            }

            template<typename F, typename ...E, std::size_t ...I>
            inline ConstructorWrapper<F, detail::DefaultArgumentManager<DefaultArgument<E, I>...>> makeConstructorWrapper(DefaultArgument<E, I> &&...defaultArguments) {
                return ConstructorWrapper<F, detail::DefaultArgumentManager<DefaultArgument<E, I>...>>(std::move(defaultArguments)...);
            }
        }
    }
}

#endif
