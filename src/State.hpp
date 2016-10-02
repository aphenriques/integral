//
//  State.hpp
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

#ifndef integral_State_hpp
#define integral_State_hpp

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <lua.hpp>
#include "exception/ClassException.hpp"
#include "core.hpp"
#include "generic.hpp"
#include "GlobalReference.hpp"
#include "Reference.hpp"
#include "IsTemplateClass.hpp"

namespace integral {
    class State {
    public:
        // throws StateException on failure
        // defines State::atPanic as lua panic function
        State();
        
        inline lua_State * getLuaState() const;
        inline void openLibs() const;
        
        // throws StateException on error
        void doString(const std::string &string) const;
    
        // throws StateException on error
        void doFile(const std::string &fileName) const;
        
        template<typename K>
        inline detail::Reference<K, detail::GlobalReference> operator[](K &&key) const;
        
    private:
        // stack argument: errorString
        static std::string getErrorMessage(lua_State *luaState);
        
        std::shared_ptr<lua_State> luaState_;
        
        // throws StateException
        [[noreturn]] static int atPanic(lua_State *luaState);
    };
    
    using StateException = exception::ClassException<State, std::runtime_error>;
    
    //--
    
    inline lua_State * State::getLuaState() const {
        return luaState_.get();
    }

    inline void State::openLibs() const {
        luaL_openlibs(getLuaState());
    }
    
    template<typename K>
    inline detail::Reference<K, detail::GlobalReference> State::operator[](K &&key) const {
        static_assert(detail::IsTemplateClass<LuaPack, detail::generic::BasicType<K>>::value == false, "integral::LuaPack cannot be a key");
        return detail::Reference<K, detail::GlobalReference>(std::forward<K>(key), detail::GlobalReference(luaState_));
    }
}

#endif
