//
//  StateView.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2016, 2017, 2019, 2020, 2021 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_StateView_hpp
#define integral_StateView_hpp

#include <string>
#include <type_traits>
#include <utility>
#include <lua.hpp>
#include <exception/ClassException.hpp>
#include <exception/Exception.hpp>
#include "core.hpp"
#include "GlobalReference.hpp"
#include "Reference.hpp"

namespace integral {
    // StateView does not own the luaState it takes
    class StateView {
    public:
        // non-copyable
        StateView(const StateView &) = delete;
        StateView & operator=(const StateView &) = delete;

        // throws StateException if luaState is nullptr
        // defines StateView::atPanic as lua panic function
        StateView(lua_State *luaState);

        // moveable
        StateView(StateView &&stateView);
        StateView & operator=(StateView &&stateView);

        inline lua_State * getLuaState() const;
        inline void openLibs() const;

        // throws StateException on error
        void doString(const std::string &string) const;

        // throws StateException on error
        void doFile(const std::string &fileName) const;

        // "detail::Reference::get" and "detail::reference::operator V" (conversion operator) throw ReferenceException
        template<typename K>
        inline detail::Reference<std::decay_t<K>, detail::GlobalReference> operator[](K &&key) const;

        template<typename D, typename B>
        inline void defineTypeFunction() const;

        template<typename F>
        inline void defineTypeFunction(F &&typeFunction) const;

        template<typename D, typename B>
        inline void defineInheritance() const;

        template<typename F>
        inline void defineInheritance(F &&typeFunction) const;

    private:
        static const char * const kErrorStackArgument;
        static const char * const kErrorStackMiscellanous;
        static const char * const kErrorStackUnspecified;

        lua_State * luaState_;

#ifdef _MSC_VER
        // throws StateException
        static int atPanic(lua_State *luaState);
#else
        // throws StateException
        [[noreturn]] static int atPanic(lua_State *luaState);
#endif
    };

    using StateException = exception::ClassException<StateView, exception::RuntimeException>;
    using ReferenceException = detail::ReferenceException;

    //--

    inline lua_State * StateView::getLuaState() const {
        return luaState_;
    }

    inline void StateView::openLibs() const {
        luaL_openlibs(getLuaState());
    }

    template<typename K>
    inline detail::Reference<std::decay_t<K>, detail::GlobalReference> StateView::operator[](K &&key) const {
        return detail::Reference<std::decay_t<K>, detail::GlobalReference>(std::forward<K>(key), detail::GlobalReference(luaState_));
    }

    template<typename D, typename B>
    inline void StateView::defineTypeFunction() const {
        integral::defineTypeFunction<D, B>(getLuaState());
    }

    template<typename F>
    inline void StateView::defineTypeFunction(F &&typeFunction) const {
        integral::defineTypeFunction(getLuaState(), std::forward<F>(typeFunction));
    }

    template<typename D, typename B>
    inline void StateView::defineInheritance() const {
        integral::defineInheritance<D, B>(getLuaState());
    }

    template<typename F>
    inline void StateView::defineInheritance(F &&typeFunction) const {
        integral::defineInheritance(getLuaState(), std::forward<F>(typeFunction));
    }
}

#endif
