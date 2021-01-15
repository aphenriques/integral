//
//  FunctorTraits.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2015, 2016, 2019, 2020, 2021 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_FunctorTraits_hpp
#define integral_FunctorTraits_hpp

#include <type_traits>
#include "FunctionSignature.hpp"

namespace integral {
    namespace detail {
        template<typename T>
        class FunctorTraits : public FunctorTraits<decltype(&std::decay_t<T>::operator())> {};

        template<typename T, typename R, typename ...A>
        class FunctorTraits<R(T::*)(A...)> : public FunctionSignature<R(A...)> {};

        template<typename T, typename R, typename ...A>
        class FunctorTraits<R(T::*)(A...) const> : public FunctionSignature<R(A...)> {};

        template<typename T, typename R, typename ...A>
        class FunctorTraits<R(T::*)(A...) noexcept> : public FunctionSignature<R(A...)> {};

        template<typename T, typename R, typename ...A>
        class FunctorTraits<R(T::*)(A...) const noexcept> : public FunctionSignature<R(A...)> {};

        template<typename T, typename R, typename ...A>
        class FunctorTraits<R(T::*)(A...) volatile> : public FunctionSignature<R(A...)> {};

        template<typename T, typename R, typename ...A>
        class FunctorTraits<R(T::*)(A...) const volatile> : public FunctionSignature<R(A...)> {};

        template<typename T, typename R, typename ...A>
        class FunctorTraits<R(T::*)(A...) volatile noexcept> : public FunctionSignature<R(A...)> {};

        template<typename T, typename R, typename ...A>
        class FunctorTraits<R(T::*)(A...) const volatile noexcept> : public FunctionSignature<R(A...)> {};
    }
}

#endif
