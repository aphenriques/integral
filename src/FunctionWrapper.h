//
//  FunctionWrapper.h
//  integral
//
//  Copyright (C) 2013, 2014  André Pereira Henriques
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

#ifndef integral_FunctionWrapper_h
#define integral_FunctionWrapper_h

#include <string>
#include <functional>
#include <exception>
#include <stdexcept>
#include <lua.hpp>
#include "UserDataWrapper.h"
#include "type_manager.h"
#include "FunctionCaller.h"
#include "TemplateSequenceGenerator.h"

namespace integral {
    namespace detail {
        template<typename R, typename ...A>
        class FunctionWrapper {
        public:
            static void setFunction(lua_State *luaState, const std::string &name, const std::function<R(A...)> &function);
            
            inline FunctionWrapper(const std::function<R(A...)> &function);
            
            inline int call(lua_State *luaState) const;
            
        private:
            std::function<R(A...)> function_;
        };
        
        //--
        
        template<typename R, typename ...A>
        void FunctionWrapper<R, A...>::setFunction(lua_State *luaState, const std::string &name, const std::function<R(A...)> &function) {
            basic::pushUserData<UserDataWrapper<FunctionWrapper<R, A...>>>(luaState, function);
            type_manager::pushClassMetatable<FunctionWrapper<R, A...>>(luaState);
            lua_setmetatable(luaState, -2);
            basic::setLuaFunction(luaState, name, [](lua_State *luaState) -> int {
                try {
                    UserDataWrapper<FunctionWrapper<R, A...>> *functionWrapper = type_manager::getUserDataWrapper<FunctionWrapper<R, A...>>(luaState, lua_upvalueindex(1));
                    if (functionWrapper != nullptr) {
                        return functionWrapper->call(luaState);
                    } else {
                        throw std::runtime_error("corrupted FunctionWrapper");
                    }
                } catch (const std::exception &exception) {
                    lua_pushstring(luaState, (std::string("[integral] ") + exception.what()).c_str());
                } catch (...) {
                    lua_pushstring(luaState, basic::gkUnknownExceptionMessage);
                }
                // error return outside catch scope so that the exception destructor can be called
                return lua_error(luaState);
            }, 1);
        }
        
        template<typename R, typename ...A>
        inline FunctionWrapper<R, A...>::FunctionWrapper(const std::function<R(A...)> &function) : function_(function) {}
        
        template<typename R, typename ...A>
        inline int FunctionWrapper<R, A...>::call(lua_State *luaState) const {
            return static_cast<int>(FunctionCaller<R, A...>::call(luaState, function_, typename TemplateSequenceGenerator<sizeof...(A)>::TemplateSequenceType()));
        }
    }
}

#endif
