//
//  VectorOfDoubles.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2015, 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#include <functional>
#include <string>
#include <vector>
#include <lua.hpp>
#include <integral/integral.hpp>

extern "C" {
    LUALIB_API int luaopen_libVectorOfDoubles(lua_State *luaState) {
        try {
            // use std::vector as a regular object type using Adaptor
            using VectorOfDoubles = integral::Adaptor<std::vector<double>>;
            integral::pushClassMetatable<VectorOfDoubles>(luaState);
            integral::setConstructor<VectorOfDoubles()>(luaState, "new");
            // because of some legacy lua implementation details, __len receives two arguments, the second argument can be safely ignored
            integral::setFunction(luaState, "__len", [](const VectorOfDoubles &vector, integral::LuaIgnoredArgument) -> std::size_t {
                return vector.size();
            });
            // explicit cast is necessary to avoid ambiguity because std::vector<T>::push_back is an overloaded function
            integral::setFunction(luaState, "pushBack", static_cast<void(VectorOfDoubles::*)(const double &)>(&VectorOfDoubles::push_back));
            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[ignored_argument sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[ignored_argument sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}
