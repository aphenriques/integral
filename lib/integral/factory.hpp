//
//  factory.hpp
//  integral
//
//  Copyright (C) 2017  André Pereira Henriques
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
