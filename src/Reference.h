//
//  Reference.h
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

#ifndef integral_Reference_h
#define integral_Reference_h

#include <memory>
#include <utility>
#include <lua.hpp>
#include "exception/Exception.h"
#include "core.h"
#include "generic.h"

namespace integral {
    namespace detail {
        template<typename T>
        class Reference {
        public:
            // non-copyable
            Reference(const Reference &) = delete;
            Reference<T> & operator=(const Reference &) = delete;
            
            inline Reference(const std::shared_ptr<lua_State>& luaState, T&& key);
            Reference(Reference &&) = default;
            
            template<typename U>
            Reference<T> & operator=(U&& value);
    
            template<typename U>
            operator U() const;
            
        private:
            std::shared_ptr<lua_State> luaState_;
            const T key_;
            
            inline lua_State * getLuaState() const;
        };
        
        //--
        
        template<typename T>
        inline Reference<T>::Reference(const std::shared_ptr<lua_State>& luaState, T&& key) : luaState_(luaState), key_(std::forward<T>(key)) {}
        
        template<typename T>
        template<typename U>
        Reference<T> & Reference<T>::operator=(U&& value) {
            lua_pushglobaltable(getLuaState());
            if (lua_istable(getLuaState(), -1) != 0) {
                // BasicType<T> must be used especially due to string literal type
                integral::push<generic::BasicType<T>>(getLuaState(), key_);
                integral::push<generic::BasicType<U>>(getLuaState(), std::forward<U>(value));
                lua_settable(getLuaState(), -3);
                return *this;
            } else {
                lua_pop(getLuaState(), -1);
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, "corrupted lua state - could not find global table");
                
            }
        }
        
        template<typename T>
        template<typename U>
        Reference<T>::operator U() const {
            lua_pushglobaltable(getLuaState());
            if (lua_istable(getLuaState(), -1) != 0) {
                // BasicType<T> must be used especially due to string literal type
                integral::push<generic::BasicType<T>>(getLuaState(), key_);
                lua_gettable(getLuaState(), -2);
                return integral::get<U>(getLuaState(), -1);
            } else {
                lua_pop(getLuaState(), -1);
                throw exception::RuntimeException(__FILE__, __LINE__, __func__, "corrupted lua state - could not find global table");

            }
        }
        
        template<typename T>
        inline lua_State * Reference<T>::getLuaState() const {
            return luaState_.get();
        }

    }
}

#endif
