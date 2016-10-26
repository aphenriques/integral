//
//  FunctorTraits.hpp
//  integral
//
//  Copyright (C) 2015, 2016  André Pereira Henriques
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

#ifndef integral_FunctorTraits_hpp
#define integral_FunctorTraits_hpp

#include "FunctionSignature.hpp"

namespace integral {
    namespace detail {
        template<typename T>
        class FunctorTraits : public FunctorTraits<decltype(&T::operator())> {};

        template<typename T, typename R, typename ...A>
        class FunctorTraits<R(T::*)(A...)> : public FunctionSignature<R(A...)> {};

        template<typename T, typename R, typename ...A>
        class FunctorTraits<R(T::*)(A...) const> : public FunctionSignature<R(A...)> {};

        template<typename T, typename R, typename ...A>
        class FunctorTraits<R(T::*)(A...) volatile> : public FunctionSignature<R(A...)> {};

        template<typename T, typename R, typename ...A>
        class FunctorTraits<R(T::*)(A...) const volatile> : public FunctionSignature<R(A...)> {};
    }
}

#endif
