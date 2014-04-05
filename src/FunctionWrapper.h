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

#include <exception>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>
#include <lua.hpp>
#include "argument.h"
#include "ArgumentException.h"
#include "basic.h"
#include "DefaultArgument.h"
#include "DefaultArgumentManager.h"
#include "FunctionCaller.h"
#include "TemplateSequenceGenerator.h"
#include "type_manager.h"
#include "UserDataWrapper.h"

namespace integral {
    namespace detail {
        template<typename M, typename R, typename ...A>
        class FunctionWrapper {
        public:
            template<typename ...E, unsigned ...I>
            static void setFunction(lua_State *luaState, const std::string &name, const std::function<R(A...)> &function, DefaultArgument<E, I> &&...defaultArguments);
            
            inline int call(lua_State *luaState) const;
            
        protected:
            template<typename ...E, unsigned ...I>
            FunctionWrapper(const std::function<R(A...)> &function, DefaultArgument<E, I> &&...defaultArguments);
            
        private:
            using FunctionWrapperType = FunctionWrapper<M, R, A...>;
            
            static constexpr unsigned keNumberOfFunctionArguments_ = sizeof...(A);

            std::function<R(A...)> function_;
            M defaultArgumentManager_;
        };
        
        //--
        
        template<typename M, typename R, typename ...A>
        template<typename ...E, unsigned ...I>
        void FunctionWrapper<M, R, A...>::setFunction(lua_State *luaState, const std::string &name, const std::function<R(A...)> &function, DefaultArgument<E, I> &&...defaultArguments) {
            argument::validateDefaultArguments<A...>(std::forward<DefaultArgument<E, I>>(defaultArguments)...);
            basic::pushUserData<UserDataWrapper<FunctionWrapperType>>(luaState, function, std::forward<DefaultArgument<E, I>>(defaultArguments)...);
            type_manager::pushClassMetatable<FunctionWrapperType>(luaState);
            lua_setmetatable(luaState, -2);
            basic::setLuaFunction(luaState, name, [](lua_State *luaState) -> int {
                try {
                    UserDataWrapper<FunctionWrapperType> *functionWrapper = type_manager::getUserDataWrapper<FunctionWrapperType>(luaState, lua_upvalueindex(1));
                    if (functionWrapper != nullptr) {
                        // replicate code of maximum number of parameters checking in function::callConstructor
                        const unsigned numberOfArgumentsOnStack = static_cast<unsigned>(lua_gettop(luaState));
                        if (numberOfArgumentsOnStack <= keNumberOfFunctionArguments_) {
                            functionWrapper->defaultArgumentManager_.processDefaultArguments(luaState, keNumberOfFunctionArguments_, numberOfArgumentsOnStack);
                            return functionWrapper->call(luaState);
                        } else {
                            throw ArgumentException(luaState, keNumberOfFunctionArguments_, numberOfArgumentsOnStack);
                        }
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
        
        template<typename M, typename R, typename ...A>
        inline int FunctionWrapper<M, R, A...>::call(lua_State *luaState) const {
            return static_cast<int>(FunctionCaller<R, A...>::call(luaState, function_, typename TemplateSequenceGenerator<keNumberOfFunctionArguments_>::TemplateSequenceType()));
        }
        
        template<typename M, typename R, typename ...A>
        template<typename ...E, unsigned ...I>
        FunctionWrapper<M, R, A...>::FunctionWrapper(const std::function<R(A...)> &function, DefaultArgument<E, I> &&...defaultArguments) : function_(function), defaultArgumentManager_(std::forward<DefaultArgument<E, I>>(defaultArguments)...) {}
    }
}

#endif
