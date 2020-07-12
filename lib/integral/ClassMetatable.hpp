//
//  ClassMetatable.hpp
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

#ifndef integral_ClassMetatable_hpp
#define integral_ClassMetatable_hpp

#include <type_traits>
#include <utility>
#include <lua.hpp>
#include "class_composite.hpp"
#include "exchanger.hpp"
#include "generic.hpp"
#include "type_manager.hpp"

namespace integral {
    // T: class type
    template<typename T>
    class ClassMetatable : public detail::class_composite::ClassCompositeInterface<T, ClassMetatable<T>> {
    public:
        // non-copyable
        ClassMetatable(const ClassMetatable &) = delete;
        ClassMetatable & operator=(const ClassMetatable &) = delete;

        ClassMetatable(ClassMetatable &&) = default;
        ClassMetatable() = default;
    };

    namespace detail {
        namespace exchanger {
            template<typename T>
            class Exchanger<ClassMetatable<T>> {
            public:
                inline static void push(lua_State *luaState);
                inline static void push(lua_State *luaState, const ClassMetatable<T> &);
            };
        }
    }

    //--

    namespace detail {
        namespace exchanger {
            template<typename T>
            inline void Exchanger<ClassMetatable<T>>::push(lua_State *luaState) {
                type_manager::pushClassMetatable<T>(luaState);
            }

            template<typename T>
            inline void Exchanger<ClassMetatable<T>>::push(lua_State *luaState, const ClassMetatable<T> &) {
                push(luaState);
            }
        }
    }
}

#endif
