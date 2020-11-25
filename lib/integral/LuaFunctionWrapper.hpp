//
//  LuaFunctionWrapper.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2013, 2014, 2015, 2016, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_LuaFunctionWrapper_hpp
#define integral_LuaFunctionWrapper_hpp

#include <functional>
#include <utility>
#include <lua.hpp>

namespace integral {
    namespace detail {
        // This class exists to make it possible to get exceptions thrown by luaFunction_ (see Exchanger<LuaFunctionWrapper>::push). For example, this is important to get exceptions thrown by exchanger::callConstructor when there are wrong parameters (exchanger::Exchanger throws).
        // The function is managed by integral so that if an exception is thrown from it, it is translated to a Lua error
        // The first upvalue in the related lua_CFunction is always the userdata that holds LuaFunctionWrapper, so the remaining upvalues indexes are offset by 1.
        // Use getUpValueIndex or lua_upvalueindex(index + 1) to get an upvalue index.
        class LuaFunctionWrapper {
        public:
            // Gets adjusted upvalue index.
            // Exchanger<LuaFunctionWrapper>::push offsets the upvalues to insert the bound function userdata in the first position.
            static inline int getUpValueIndex(int index);

            // "luaFunction": lua_CFunction like function or functor.
            template<typename T>
            inline LuaFunctionWrapper(T &&luaFunction);

            // necessary because of the template constructor
            LuaFunctionWrapper(const LuaFunctionWrapper &) = default;
            LuaFunctionWrapper(LuaFunctionWrapper &) = default;
            LuaFunctionWrapper(LuaFunctionWrapper &&) = default;

            inline const std::function<int(lua_State *)> & getLuaFunction() const;

        private:
            std::function<int(lua_State *)> luaFunction_;
        };

        // exchanger::Exchanger<LuaFunctionWrapper> is in exceptions.cpp because exchanger::push depends on it

        //--

        inline int LuaFunctionWrapper::getUpValueIndex(int index) {
            return lua_upvalueindex(index + 1);
        }

        template<typename T>
        inline LuaFunctionWrapper::LuaFunctionWrapper(T &&luaFunction) : luaFunction_(std::forward<T>(luaFunction)) {}

        inline const std::function<int(lua_State *)> & LuaFunctionWrapper::getLuaFunction() const {
            return luaFunction_;
        }
    }
}

#endif
