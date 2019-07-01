//
//  StateView.hpp
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
        inline detail::Reference<typename std::decay<K>::type, detail::GlobalReference> operator[](K &&key) const;

        template<typename F>
        inline void defineTypeFunction(F &&typeFunction) const;

        template<typename D, typename B>
        inline void defineInheritance() const;

        template<typename F>
        inline void defineInheritance(F &&typeFunction) const;

        template<typename T>
        inline void defineReferenceWrapperInheritance() const;

        template<typename T>
        inline void defineSharedPtrInheritance() const;

    private:
        static const char * const kErrorStackArgument;
        static const char * const kErrorStackMiscellanous;
        static const char * const kErrorStackUnspecified;

        lua_State * luaState_;

        // throws StateException
        [[noreturn]] static int atPanic(lua_State *luaState);
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
    inline detail::Reference<typename std::decay<K>::type, detail::GlobalReference> StateView::operator[](K &&key) const {
        return detail::Reference<typename std::decay<K>::type, detail::GlobalReference>(std::forward<K>(key), detail::GlobalReference(luaState_));
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

    template<typename T>
    inline void StateView::defineReferenceWrapperInheritance() const {
        integral::defineReferenceWrapperInheritance<T>(getLuaState());
    }

    template<typename T>
    inline void StateView::defineSharedPtrInheritance() const {
        integral::defineSharedPtrInheritance<T>(getLuaState());
    }
}

#endif
