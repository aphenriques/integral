//
//  FunctionTraits.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2016, 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_FunctionTraits_hpp
#define integral_FunctionTraits_hpp

#include "FunctionSignature.hpp"
#include "FunctorTraits.hpp"

namespace integral {
    namespace detail {
        template<typename T>
        class FunctionTraits : public FunctorTraits<T> {};

        template<typename R, typename ...A>
        class FunctionTraits<std::function<R(A...)>> : public FunctionSignature<R(A...)> {};

        template<typename R, typename ...A>
        class FunctionTraits<R(*)(A...)> : public FunctionSignature<R(A...)> {};

        template<typename R, typename ...A>
        class FunctionTraits<R(&)(A...)> : public FunctionSignature<R(A...)> {};

        template<typename R, typename T, typename ...A>
        class FunctionTraits<R(T::*)(A...)> : public FunctionSignature<R(T &, A...)> {};

        template<typename R, typename T, typename ...A>
        class FunctionTraits<R(T::*)(A...) const> : public FunctionSignature<R(const T &, A...)> {};

        template<typename R, typename T, typename ...A>
        class FunctionTraits<R(T::*)(A...) volatile> : public FunctionSignature<R(volatile T &, A...)> {};

        template<typename R, typename T, typename ...A>
        class FunctionTraits<R(T::*)(A...) const volatile> : public FunctionSignature<R(const volatile T &, A...)> {};

        template<typename R, typename ...A>
        class FunctionTraits<R(*)(A...) noexcept> : public FunctionSignature<R(A...)> {};

        template<typename R, typename ...A>
        class FunctionTraits<R(&)(A...) noexcept> : public FunctionSignature<R(A...)> {};

        template<typename R, typename T, typename ...A>
        class FunctionTraits<R(T::*)(A...) noexcept> : public FunctionSignature<R(T &, A...)> {};

        template<typename R, typename T, typename ...A>
        class FunctionTraits<R(T::*)(A...) const noexcept> : public FunctionSignature<R(const T &, A...)> {};

        template<typename R, typename T, typename ...A>
        class FunctionTraits<R(T::*)(A...) volatile noexcept> : public FunctionSignature<R(volatile T &, A...)> {};

        template<typename R, typename T, typename ...A>
        class FunctionTraits<R(T::*)(A...) const volatile noexcept> : public FunctionSignature<R(const volatile T &, A...)> {};
    }
}

#endif
