//
//  IsTemplateClass.h
//  integral
//
//  Copyright (C) 2016  André Pereira Henriques
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

#ifndef IsTemplateClass_h
#define IsTemplateClass_h

#include <type_traits>

namespace integral {
    namespace detail {
        template<template<typename...> class T, typename U>
        class IsTemplateClass : public std::false_type {};
        
        template<template<typename...> class T, typename ...P>
        class IsTemplateClass<T, T<P...>> : public std::true_type {};
    }
}

#endif /* IsTemplateClass_h */
