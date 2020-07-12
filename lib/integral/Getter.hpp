//
//  Getter.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2017, 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_Getter_hpp
#define integral_Getter_hpp

namespace integral {
    namespace detail {
        template<typename T, typename R>
        class Getter {
        public:
            inline Getter(R T::* attribute);

            inline R operator()(const T &object) const;

        private:
            R T::* const attribute_;
        };

        //--

        template<typename T, typename R>
        inline Getter<T, R>::Getter(R T::* attribute) : attribute_(attribute) {}

        template<typename T, typename R>
        inline R Getter<T, R>::operator()(const T &object) const {
            return object.*attribute_;
        }
    }
}

#endif
