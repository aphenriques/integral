//
//  state.cpp
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

#include <iostream>
#include <memory>
#include <lua.hpp>
#include <integral.hpp>

static const char * const luaTestCode = R"(
print("hello from string sample code")
)";

int main(int argc, char* argv[]) {
    try {
        std::cout << "State:\n";
        integral::State luaState;
        luaState.openLibs();
        luaState.doFile("sample.lua");
        luaState.doString(luaTestCode);

        std::cout << "StateView:\n";
        std::unique_ptr<lua_State, decltype(&lua_close)> luaStateUniquePtr(luaL_newstate(), &lua_close);
        // lua_State pointer ownership is NOT transfered to luaStateView (it remains with luaStateUniquePtr)
        integral::StateView luaStateView(luaStateUniquePtr.get()); 
        luaStateView.openLibs();
        luaStateView.doFile("sample.lua");
        luaStateView.doString(luaTestCode);
        
        try {
            integral::StateView(nullptr);
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }

        try {
            luaState.doString("invalid_statement");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }

        try {
            luaState.doFile("nonexistentFile");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }
        
        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[state] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}
