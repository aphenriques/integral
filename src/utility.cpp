//
//  utility.cpp
//  integral
//
//  Copyright (C) 2013, 2014, 2015, 2016  André Pereira Henriques
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

#include "utility.h"
#include <functional>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <tuple>
#include "core.h"

namespace integral {
    namespace utility {
        namespace {
            const char * const gkHelpKey = "help";
        }

        int printStack(lua_State *luaState) {
            int top = lua_gettop(luaState);
            std::cout << "stack dump: ";
            for (int i = 1; i <= top; ++i) {
                int t = lua_type(luaState, i);
                switch (t) {
                    case LUA_TSTRING:
                        std::cout << '"' << lua_tostring(luaState, i) << '"';
                        break;
                        
                    case LUA_TBOOLEAN:
                        std::cout <<(lua_toboolean(luaState, i) ? "true" : "false");
                        break;
                        
                    case LUA_TNUMBER:
                        std::cout << lua_tonumber(luaState, i);
                        break;
                        
                    case LUA_TLIGHTUSERDATA:
                        std::cout << "lightuserdata";
                        break;
                        
                    default:
                        if (lua_iscfunction(luaState, i) != 0) {
                            std::cout << "cfunction";
                        } else {
                            std::cout << lua_typename(luaState, t);
                        }
                        break;
                        
                }
                std::cout << "  ";
            }
            std::cout << std::endl;
            return 0;
        }
        
        void setHelp(lua_State *luaState, const char *field, const char *fieldDescription) {
            lua_pushstring(luaState, gkHelpKey);
            lua_rawget(luaState, -2);
            if (lua_isnil(luaState, -1) != 0) {
                lua_pop(luaState, 1);
                lua_newtable(luaState);
                lua_pushvalue(luaState, -1);
                lua_pushstring(luaState, gkHelpKey);
                lua_insert(luaState, -2);
                lua_rawset(luaState, -4);
            }
            lua_pushstring(luaState, field);
            lua_pushstring(luaState, fieldDescription);
            lua_rawset(luaState, -3);
            lua_pop(luaState, 1);
        }
        
        void setWithHelp(lua_State *luaState, const char *field, const char *fieldDescription, const std::function<void(lua_State *)> &pushFunction) {
            lua_pushstring(luaState, field);
            pushFunction(luaState);
            lua_rawset(luaState, -3);
            setHelp(luaState, field, fieldDescription);
        }
        
        void pushNameAndValueList(lua_State *luaState, std::initializer_list<std::tuple<const char *, int>> nameAndValueList) {
            lua_newtable(luaState);
            for (auto &nameAndValue : nameAndValueList) {
                integral::push<const char *>(luaState, std::get<0>(nameAndValue));
                integral::push<int>(luaState, std::get<1>(nameAndValue));
                lua_rawset(luaState, -3);
            }
        }
        
        void setNameAndValueListWithHelp(lua_State *luaState, const char *field, std::initializer_list<std::tuple<const char *, int>> nameAndValueList) {
            std::ostringstream fieldDescription;
            fieldDescription << '{';
            if (nameAndValueList.size() != 0) {
                auto nameAndValueListIterator = nameAndValueList.begin();
                const auto nameAndValueListIteratorEndMinus1 = nameAndValueList.end() - 1;
                while (nameAndValueListIterator != nameAndValueListIteratorEndMinus1) {
                    fieldDescription << std::get<0>(*nameAndValueListIterator) << ", ";
                    ++nameAndValueListIterator;
                }
                fieldDescription << std::get<0>(*nameAndValueListIteratorEndMinus1);
            }
            fieldDescription << '}';
            setWithHelp(luaState, field, fieldDescription.str().c_str(), [&nameAndValueList](lua_State *luaState) {
                pushNameAndValueList(luaState, nameAndValueList);
            });
        }
    }
}
