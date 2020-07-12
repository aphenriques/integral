//
//  DefaultArgumentManagerContainer.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_DefaultArgumentManagerContainer_hpp
#define integral_DefaultArgumentManagerContainer_hpp

#include <cstddef>
#include <utility>
#include "argument.hpp"
#include "DefaultArgumentManager.hpp"
#include "IsTemplateClass.hpp"

namespace integral {
    namespace detail {
        template<typename T, typename ...A>
        class DefaultArgumentManagerContainer {
            static_assert(IsTemplateClass<DefaultArgumentManager, T>::value == true, "typename T in DefaultArgumentManagerContainer must be a DefaultArgumentManager");
        public:
            template<typename ...E, std::size_t ...I>
            inline DefaultArgumentManagerContainer(DefaultArgument<E, I> &&...defaultArguments);

            inline const T & getDefaultArgumentManager() const;

        private:
            T defaultArgumentManager_;
        };

        //--

        template<typename T, typename ...A>
        template<typename ...E, std::size_t ...I>
        inline DefaultArgumentManagerContainer<T, A...>::DefaultArgumentManagerContainer(DefaultArgument<E, I> &&...defaultArguments) : defaultArgumentManager_(std::move(defaultArguments)...) {
            argument::validateDefaultArguments<A...>(defaultArguments...);
        }

        template<typename T, typename ...A>
        inline const T & DefaultArgumentManagerContainer<T, A...>::getDefaultArgumentManager() const {
            return defaultArgumentManager_;
        }
    }
}

#endif /* integral_DefaultArgumentManagerContainer_hpp */
