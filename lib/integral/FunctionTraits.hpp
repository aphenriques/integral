//
//  FunctionTraits.hpp
//  integral
//
//  Copyright (C) 2016, 2019  André Pereira Henriques
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
