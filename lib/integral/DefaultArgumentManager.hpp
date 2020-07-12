//
//  DefaultArgumentManager.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2015, 2016, 2017, 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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
