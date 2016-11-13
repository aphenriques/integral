//
//  StateView.hpp
//  integral
//
//  Copyright (C) 2016  André Pereira Henriques
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
#include <utility>
#include <lua.hpp>
#include "exception/ClassException.hpp"
#include "exception/Exception.hpp"
#include "Adaptor.hpp"
#include "GlobalReference.hpp"
#include "Reference.hpp"

namespace integral {
    // StateView does not own the luaState it takes
    class StateView {
    public:
        // non-copyable
        StateView(const StateView &) = delete;
        StateView & operator=(const StateView &) = delete;
        
        // throws exception::RuntimeException if luaState is nullptr
        // defines StateView::atPanic as lua panic function
        StateView(lua_State *luaState);
        
        inline lua_State * getLuaState() const;
        inline void openLibs() const;

        // throws StateException on error
        void doString(const std::string &string) const;

        // throws StateException on error
        void doFile(const std::string &fileName) const;

        template<typename K>
        inline detail::Reference<K, detail::GlobalReference> operator[](K &&key) const;

    private:
        static const char * const kErrorStackArgument;
        static const char * const kErrorStackMiscellanous;
        static const char * const kErrorStackUnspecified;

        lua_State * const luaState_;

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
    inline detail::Reference<K, detail::GlobalReference> StateView::operator[](K &&key) const {
        // Adaptor<detail::Reference<...>> is utilized to access protected constructors of detail::Reference<...> and detail::GlobalReference
        return detail::Adaptor<detail::Reference<K, detail::GlobalReference>>(std::forward<K>(key), detail::Adaptor<detail::GlobalReference>(luaState_));
    }
}

#endif