//
//  serializer.hpp
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

#ifndef integral_serializer_hpp
#define integral_serializer_hpp

#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>

namespace integral {
    namespace detail {
        namespace serializer {
            template <typename T, typename Enable = void>
            class Serializer {
            public:
                static std::string getString(const T &t);
            };

            template <typename T>
            class Serializer<T, typename std::enable_if<std::is_arithmetic<T>::value>::type> {
            public:
                static std::string getString(const T &value);
            };

            template <typename T>
            class Serializer<T, typename std::enable_if<std::is_same<T, std::string>::value || std::is_same<T, const char *>::value>::type> {
            public:
                static std::string getString(const T &value);
            };

            template<typename T>
            inline std::string getString(T &&t);
            
            //--
            
            template <typename T, typename Enable>
            std::string Serializer<T, Enable>::getString(const T &t) {
                std::ostringstream stringStream;
                stringStream << "object:["<< typeid(t).name() << "]";
                return stringStream.str();
            }

            template<typename T>
            std::string Serializer<T, typename std::enable_if<std::is_arithmetic<T>::value>::type>::getString(const T &value) {
                std::ostringstream stringStream;
                stringStream << std::boolalpha << value;
                return stringStream.str();
            }

            template<typename T>
            std::string Serializer<T, typename std::enable_if<std::is_same<T, std::string>::value || std::is_same<T, const char *>::value>::type>::getString(const T &value) {
                std::ostringstream stringStream;
                stringStream << '"' << value << '"';
                return stringStream.str();
            }
            
            template<typename T>
            inline std::string getString(T &&t) {
                return Serializer<typename std::decay<T>::type>::getString(std::forward<T>(t));
            }
        }
    }
}

#endif /* integral_serializer_hpp */
