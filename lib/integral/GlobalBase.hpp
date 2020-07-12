//
//  GlobalBase.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_GlobalBase_hpp
#define integral_GlobalBase_hpp

#include <string>
#include <lua.hpp>
#include "lua_compatibility.hpp"

namespace integral {
    class GlobalBase {
    public:
        // non-copyable
        GlobalBase(const GlobalBase &) = delete;
        GlobalBase & operator=(const GlobalBase &) = delete;

        GlobalBase(GlobalBase &&) = default;
        GlobalBase() = default;

        inline std::string getReferenceString() const;

    protected:
        inline void push(lua_State *luaState) const;
    };

    //--

    inline std::string GlobalBase::getReferenceString() const {
        return "_G";
    }

    inline void GlobalBase::push(lua_State *luaState) const {
        detail::lua_compatibility::pushglobaltable(luaState);
    }
}

#endif

