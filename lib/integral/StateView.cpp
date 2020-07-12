//
//  StateView.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2016, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#include "StateView.hpp"
#include <exception>
#include <string>
#include <lua.hpp>
#include <exception/Exception.hpp>
#include "ArgumentException.hpp"
#include "core.hpp"
#include "lua_compatibility.hpp"

namespace integral {
    StateView::StateView(lua_State *luaState) : luaState_(luaState) {
        if (luaState_ != nullptr) {
            lua_atpanic(getLuaState(), &StateView::atPanic);
        } else {
            throw StateException(__FILE__, __LINE__, __func__, "[integral] nullptr passed to StateView::StateView(lua_State *luaState)");
        }
    }

    StateView::StateView(StateView &&stateView) : StateView(stateView.luaState_) {
        stateView.luaState_ = nullptr;
    }

    StateView & StateView::operator=(StateView &&stateView) {
        luaState_ = stateView.luaState_;
        stateView.luaState_ = nullptr;
        return *this;
    }

    void StateView::doString(const std::string &string) const {
        if (luaL_dostring(getLuaState(), string.c_str()) != detail::lua_compatibility::keLuaOk) {
            std::string errorMessage;
            try {
                errorMessage = integral::get<std::string>(getLuaState(), -1);
            } catch (const ArgumentException &argumentException) {
                lua_pop(getLuaState(), 1);
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string(kErrorStackArgument) + argumentException.what() + " }");
            } catch (const std::exception &exception) {
                lua_pop(getLuaState(), 1);
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string(kErrorStackMiscellanous) + exception.what() + " }");
            } catch (...) {
                lua_pop(getLuaState(), 1);
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, kErrorStackUnspecified);
            }
            lua_pop(getLuaState(), 1);
            throw StateException(__FILE__, __LINE__, __func__, std::string("[integral] ") + errorMessage);
        }
    }

    void StateView::doFile(const std::string &fileName) const {
        if (luaL_dofile(getLuaState(), fileName.c_str()) != detail::lua_compatibility::keLuaOk) {
            std::string errorMessage;
            try {
                errorMessage = integral::get<std::string>(getLuaState(), -1);
            } catch (const ArgumentException &argumentException) {
                lua_pop(getLuaState(), 1);
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string(kErrorStackArgument) + argumentException.what() + " }");
            } catch (const std::exception &exception) {
                lua_pop(getLuaState(), 1);
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string(kErrorStackMiscellanous) + exception.what() + " }");
            } catch (...) {
                lua_pop(getLuaState(), 1);
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, kErrorStackUnspecified);
            }
            lua_pop(getLuaState(), 1);
            throw StateException(__FILE__, __LINE__, __func__, std::string("[integral] ") + errorMessage);
        }
    }

    int StateView::atPanic(lua_State *luaState) {
        std::string errorMessage;
        try {
            errorMessage = integral::get<std::string>(luaState, -1);
        } catch (const ArgumentException &argumentException) {
            lua_pop(luaState, 1);
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string(kErrorStackArgument) + argumentException.what() + " }");
        } catch (const std::exception &exception) {
            lua_pop(luaState, 1);
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string(kErrorStackMiscellanous) + exception.what() + " }");
        } catch (...) {
            lua_pop(luaState, 1);
            throw exception::RuntimeException(__FILE__, __LINE__, __func__, kErrorStackUnspecified);
        }
        lua_pop(luaState, 1);
        throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string("[integral] lua panic error: ") + errorMessage);
    }

    const char * const StateView::kErrorStackArgument = "[integral] could not retrieve error message from lua stack: { ";
    const char * const StateView::kErrorStackMiscellanous = "[integral] miscellaneous exception thrown retrieving error message from lua stack: { ";
    const char * const StateView::kErrorStackUnspecified = "[integral] unspecified exception thrown retrieving error message from lua stack";
}
