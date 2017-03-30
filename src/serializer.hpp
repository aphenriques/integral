//
//  serializer.hpp
//  integral
//
//  Copyright (C) 2016, 2017  André Pereira Henriques
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
