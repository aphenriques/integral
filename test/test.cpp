//
//  test.cpp
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

#include <cstring>
#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <catch.hpp>
#include <core.hpp>
#include <utility.hpp>

class Object {
public:
    Object(const std::string &id) : id_(id) {}

    bool operator==(const Object &object) const {
        return getId()==object.getId();
    }

    const std::string & getId() const {
        return id_;
    }
private:
    std::string id_;
};

TEST_CASE("Core Test", "[core]") {
    std::unique_ptr<lua_State, decltype(&lua_close)> luaState(luaL_newstate(), &lua_close);
    REQUIRE(luaState.get() != nullptr);
    SECTION("testing integral::get and integral::push") {
        integral::push<int>(luaState.get(), -42);
        integral::push<unsigned>(luaState.get(), 42u);
        integral::push<double>(luaState.get(), 42.1);
        integral::push<const char *>(luaState.get(), "string1");
        integral::push<std::string>(luaState.get(), "string2");
        integral::push<bool>(luaState.get(), true);
        integral::push<bool>(luaState.get(), false);
        integral::push<Object>(luaState.get(), "object");
        REQUIRE(integral::get<int>(luaState.get(), -8) == -42);
        REQUIRE(integral::get<int>(luaState.get(), 1) == -42);
        REQUIRE(integral::get<unsigned>(luaState.get(), -7) == 42u);
        REQUIRE(integral::get<unsigned>(luaState.get(), 2) == 42u);
        REQUIRE(integral::get<double>(luaState.get(), -6) == 42.1);
        REQUIRE(integral::get<double>(luaState.get(), 3) == 42.1);
        REQUIRE(std::strcmp(integral::get<const char *>(luaState.get(), -5), "string1") == 0);
        REQUIRE(std::strcmp(integral::get<const char *>(luaState.get(), 4), "string1") == 0);
        REQUIRE(integral::get<std::string>(luaState.get(), -5) == "string1");
        REQUIRE(integral::get<std::string>(luaState.get(), -4) == "string2");
        REQUIRE(integral::get<std::string>(luaState.get(), 5) == "string2");
        REQUIRE(std::strcmp(integral::get<const char *>(luaState.get(), -4), "string2") == 0);
        REQUIRE(integral::get<bool>(luaState.get(), -3) == true);
        REQUIRE(integral::get<bool>(luaState.get(), 6) == true);
        REQUIRE(integral::get<bool>(luaState.get(), -2) == false);
        REQUIRE(integral::get<bool>(luaState.get(), 7) == false);
        REQUIRE(integral::get<Object>(luaState.get(), -1).getId() == "object");
        lua_pop(luaState.get(), 8);
    }
    SECTION("testing integral::get incompatible types") {
        integral::push<std::string>(luaState.get(), "string");
        REQUIRE_THROWS_AS(integral::get<int>(luaState.get(), -1), integral::ArgumentException);
        REQUIRE_THROWS_AS(integral::get<Object>(luaState.get(), -1), integral::ArgumentException);
        lua_pop(luaState.get(), 1);
        integral::push<Object>(luaState.get(), "object");
        REQUIRE_THROWS_AS(integral::get<int>(luaState.get(), -1), integral::ArgumentException);
        REQUIRE_THROWS_AS(integral::get<std::string>(luaState.get(), -1), integral::ArgumentException);
        lua_pop(luaState.get(), 1);
    }

    SECTION("integral::push and integral::get with Adaptors") {
        const std::vector<int> vector{1, 2, 3};
        integral::push<integral::LuaVector<int>>(luaState.get(), vector);
        REQUIRE(integral::get<integral::LuaVector<int>>(luaState.get(), -1) == vector);
        lua_pop(luaState.get(), 1);
        std::vector<std::vector<Object>> vectorOfVectors{{Object("object[0][0]")}, {Object("object[1][0]"), Object("object[1][1]")}};
        integral::LuaVector<integral::LuaVector<Object>> vectorOfVectorsLua(vectorOfVectors.begin(), vectorOfVectors.end());
        integral::push<integral::LuaVector<integral::LuaVector<Object>>>(luaState.get(), vectorOfVectorsLua);
        REQUIRE(integral::get<integral::LuaVector<integral::LuaVector<Object>>>(luaState.get(), -1) == vectorOfVectorsLua);
        lua_pop(luaState.get(), 1);
        // TODO
    }

//    SECTION("setting and calling functions") {
//         // TODO don't forget adaptors
//    }

    REQUIRE(lua_gettop(luaState.get()) == 0);
}
