//
//  Setter.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_Setter_hpp
#define integral_Setter_hpp

namespace integral {
    namespace detail {
        template<typename T, typename R>
        class Setter {
        public:
            inline Setter(R T::* attribute);

            inline void operator()(T &object, const R &value) const;

        private:
            R T::* const attribute_;
        };

        //--

        template<typename T, typename R>
        inline Setter<T, R>::Setter(R T::* attribute) : attribute_(attribute) {}

        template<typename T, typename R>
        inline void Setter<T, R>::operator()(T &object, const R &value) const {
            object.*attribute_ = value;
        }
    }
}

#endif
