//
//  DefaultArgument.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2016, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_DefaultArgument_hpp
#define integral_DefaultArgument_hpp

#include <cstddef>
#include <utility>
#include "ArgumentTag.hpp"

namespace integral {
    // "T": type of default argument
    // "I": lua index (starts with 1) of the default argument
    template<typename T, std::size_t I>
    class DefaultArgument {
    public:
        using ArgumentTag = detail::ArgumentTag<T, I>;

        // Arguments are forwarded to typename T constructor
        template<typename ...A>
        inline DefaultArgument(A &&...arguments);

        // Avoids template constructor ambiguity
        DefaultArgument(const DefaultArgument<T, I> &) = default;
        DefaultArgument(DefaultArgument<T, I> &) = default;
        DefaultArgument(DefaultArgument<T, I> &&) = default;

        inline const T & getArgument() const;

    private:
        T argument_;
    };

    //--

    template<typename T, std::size_t I>
    template<typename ...A>
    inline DefaultArgument<T, I>::DefaultArgument(A &&...arguments) : argument_(std::forward<A>(arguments)...) {}

    template<typename T, std::size_t I>
    inline const T & DefaultArgument<T, I>::getArgument() const {
        return argument_;
    }

}

#endif
