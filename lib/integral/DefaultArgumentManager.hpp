//
//  DefaultArgumentManager.hpp
//  integral
//
//  Copyright (C) 2014, 2015, 2016, 2017, 2019  André Pereira Henriques
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

#ifndef integral_DefaultArgumentManager_hpp
#define integral_DefaultArgumentManager_hpp

#include <cstddef>
#include <tuple>
#include <utility>
#include <lua.hpp>
#include "DefaultArgument.hpp"
#include "exchanger.hpp"
#include "generic.hpp"

namespace integral {
    namespace detail {
        template<typename ...F>
        class DefaultArgumentManager {
        public:
            template<typename ...E, std::size_t ...I>
            inline DefaultArgumentManager(DefaultArgument<E, I> &&...defaultArguments);

            void processDefaultArguments(lua_State *luaState, const std::size_t numberOfFunctionArguments, const std::size_t numberOfArgumentsOnStack) const;

        private:            
            std::tuple<F...> defaultArguments_;

            template<std::size_t ...S>
            inline void processDefaultArguments(lua_State *luaState, std::index_sequence<S...>) const;

            template<typename E, std::size_t I>
            void processArgument(lua_State *luaState, const DefaultArgument<E, I> &defaultArgument) const;
        };

        //--

        template<typename ...F>
        template<typename ...E, std::size_t ...I>
        inline DefaultArgumentManager<F...>::DefaultArgumentManager(DefaultArgument<E, I> &&...defaultArguments) : defaultArguments_(std::move(defaultArguments)...) {}

        template<typename ...F>
        void DefaultArgumentManager<F...>::processDefaultArguments(lua_State *luaState, const std::size_t numberOfFunctionArguments, const std::size_t numberOfArgumentsOnStack) const {
            if (numberOfArgumentsOnStack < numberOfFunctionArguments) {
                const std::size_t numberOfArgumentsDifference = numberOfFunctionArguments - numberOfArgumentsOnStack;
                for (std::size_t i = 0; i < numberOfArgumentsDifference; ++i) {
                    lua_pushnil(luaState);
                }
            }
            processDefaultArguments(luaState, std::index_sequence_for<F...>());
        }

        template<typename ...F>
        template<std::size_t ...S>
        // [[maybe_unused]] is used because of a bug in gcc 7.4 which incorrectly shows the following warning:
        // error: parameter ‘luaState’ set but not used [-Werror=unused-but-set-parameter]
        // FIXME remove [[maybe_unused]] in future versions
        inline void DefaultArgumentManager<F...>::processDefaultArguments([[maybe_unused]] lua_State *luaState, std::index_sequence<S...>) const {
            generic::expandDummyTemplatePack((processArgument(luaState, std::get<S>(defaultArguments_)), 0)...);
        }

        template<typename ...F>
        template<typename E, std::size_t I>
        void DefaultArgumentManager<F...>::processArgument(lua_State *luaState, const DefaultArgument<E, I> &defaultArgument) const {
            if (lua_isnil(luaState, I) != 0) {
                exchanger::push<E>(luaState, defaultArgument.getArgument());
                lua_replace(luaState, I);
            }
        }
    }
}

#endif
