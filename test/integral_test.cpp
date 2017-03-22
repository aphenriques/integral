//
//  integral_test.cpp
//  integral
//
//  Copyright (C) 2017  André Pereira Henriques
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
#include <integral.hpp>
#include <exception/Exception.hpp>

class Object {
public:
    bool flag_ = false;

    Object() = default;
    Object(const std::string &id) : id_(id) {}
    Object(unsigned id) : id_(std::to_string(id)) {}

    bool operator==(const Object &object) const {
        return getId()==object.getId() && flag_ == object.flag_;
    }

    const std::string & getId() const {
        return id_;
    }
    
    void setId(const std::string &id) {
        id_ = id;
    }


private:
    std::string id_;
};

Object makeObject(const std::string &id) {
    return id;
}

double getSum(double x, double y) {
    return x + y;
}

TEST_CASE("integral test") {
    std::unique_ptr<lua_State, decltype(&lua_close)> luaState(luaL_newstate(), &lua_close);
    REQUIRE(luaState.get() != nullptr);
    integral::StateView stateView(luaState.get());
    stateView.openLibs();
    SECTION("integral::get and integral::push") {
        integral::push<int>(luaState.get(), -42);
        integral::push<unsigned>(luaState.get(), 42u);
        integral::push<double>(luaState.get(), 42.1);
        integral::push<const char *>(luaState.get(), "string1");
        integral::push<std::string>(luaState.get(), "string2");
        const std::string stringWithNullCharacter("string\0with null", 17);
        integral::push<std::string>(luaState.get(), stringWithNullCharacter);
        integral::push<bool>(luaState.get(), true);
        integral::push<bool>(luaState.get(), false);
        integral::push<Object>(luaState.get(), "object");
        REQUIRE(integral::get<int>(luaState.get(), -9) == -42);
        REQUIRE(integral::get<int>(luaState.get(), 1) == -42);
        REQUIRE(integral::get<unsigned>(luaState.get(), -8) == 42u);
        REQUIRE(integral::get<unsigned>(luaState.get(), 2) == 42u);
        REQUIRE(integral::get<double>(luaState.get(), -7) == 42.1);
        REQUIRE(integral::get<double>(luaState.get(), 3) == 42.1);
        REQUIRE(std::strcmp(integral::get<const char *>(luaState.get(), -6), "string1") == 0);
        REQUIRE(std::strcmp(integral::get<const char *>(luaState.get(), 4), "string1") == 0);
        REQUIRE(integral::get<std::string>(luaState.get(), -6) == "string1");
        REQUIRE(integral::get<std::string>(luaState.get(), -5) == "string2");
        REQUIRE(integral::get<std::string>(luaState.get(), 5) == "string2");
        REQUIRE(std::strcmp(integral::get<const char *>(luaState.get(), -5), "string2") == 0);
        REQUIRE(integral::get<std::string>(luaState.get(), -4) == stringWithNullCharacter);
        REQUIRE(integral::get<std::string>(luaState.get(), 6) == stringWithNullCharacter);
        REQUIRE(integral::get<bool>(luaState.get(), -3) == true);
        REQUIRE(integral::get<bool>(luaState.get(), 7) == true);
        REQUIRE(integral::get<bool>(luaState.get(), -2) == false);
        REQUIRE(integral::get<bool>(luaState.get(), 8) == false);
        REQUIRE(integral::get<Object>(luaState.get(), -1).getId() == "object");
        REQUIRE(integral::get<Object>(luaState.get(), 9).getId() == "object");
        lua_pop(luaState.get(), 9);
    }
    SECTION("integral::get incompatible types") {
        integral::push<std::string>(luaState.get(), "string");
        REQUIRE_THROWS_AS(integral::get<int>(luaState.get(), -1), integral::ArgumentException);
        REQUIRE_THROWS_AS(integral::get<Object>(luaState.get(), -1), integral::ArgumentException);
        lua_pop(luaState.get(), 1);
        integral::push<Object>(luaState.get(), "object");
        REQUIRE_THROWS_AS(integral::get<int>(luaState.get(), -1), integral::ArgumentException);
        REQUIRE_THROWS_AS(integral::get<std::string>(luaState.get(), -1), integral::ArgumentException);
        lua_pop(luaState.get(), 1);
    }
    SECTION("integral::StateView::doString and integral::StateView::doFile") {
        REQUIRE_NOTHROW(stateView.doString("assert(42 == 42)"));
        REQUIRE_THROWS_AS(stateView.doString("assert(42 ~= 42)"), integral::StateException);
        REQUIRE_NOTHROW(stateView.doFile("doFileTest.lua"));
        REQUIRE_THROWS_AS(stateView.doFile("nonexistentFile"), integral::StateException);
    }
    SECTION("integral::detail::Reference::get and integral::detail::Reference::set") {
        stateView["x"].set(-42);
        REQUIRE(stateView["x"].get<int>() == -42);
        stateView["x"].set(42u);
        REQUIRE(stateView["x"].get<unsigned>() == 42u);
        stateView["x"].set(42.1);
        REQUIRE(stateView["x"].get<double>() == 42.1);
        stateView["x"].set("string1");
        REQUIRE(std::strcmp(stateView["x"].get<const char *>(), "string1") == 0);
        stateView["x"].set(std::string("string2"));
        REQUIRE(stateView["x"].get<std::string>() == "string2");
        const std::string stringWithNullCharacter("string\0with null", 17);
        stateView["x"].set(stringWithNullCharacter);
        REQUIRE(stateView["x"].get<std::string>() == stringWithNullCharacter);
        stateView["x"].set(true);
        REQUIRE(stateView["x"].get<bool>() == true);
        stateView["x"].set(false);
        REQUIRE(stateView["x"].get<bool>() == false);
        stateView["x"].set(Object("object"));
        REQUIRE(stateView["x"].get<Object>() == Object("object"));
    }
    SECTION("integral::detail::Reference::get incompatible types") {
        stateView["x"].set("string");
        REQUIRE_THROWS_AS(stateView["x"].get<int>(), integral::ReferenceException);
        REQUIRE_THROWS_AS(stateView["x"].get<Object>(), integral::ReferenceException);
        stateView["x"].set(Object("object"));
        REQUIRE_THROWS_AS(stateView["x"].get<int>(), integral::ReferenceException);
        REQUIRE_THROWS_AS(stateView["x"].get<std::string>(), integral::ReferenceException);
    }
    SECTION("integral::setFunction and integral::setLuaFunction") {
        lua_newtable(luaState.get());
        integral::setFunction(luaState.get(), "getSum", getSum);
        integral::setFunction(luaState.get(), "makeObject", makeObject);
        integral::setFunction(luaState.get(), "getObjectId", std::function<std::string(const Object &)>(&Object::getId));
        integral::setFunction(luaState.get(), "getMultiplicationFunction1", [](int x) -> integral::FunctionWrapper<int(int)> {
            return integral::FunctionWrapper<int(int)>([x](int y) -> int {
                return x*y;
            });
        });
        integral::setFunction(luaState.get(), "getMultiplicationFunction2", [](int x) -> integral::LuaFunctionWrapper {
            return integral::LuaFunctionWrapper([x](lua_State *lambdaLuaState) -> int {
                const int y = integral::get<int>(lambdaLuaState, 1);
                integral::push<int>(lambdaLuaState, x*y);
                return 1;
            });
        });
        integral::setLuaFunction(luaState.get(), "getMultiplicationFunction3", [](lua_State *lambdaLuaState) -> int {
            const int x = integral::get<int>(lambdaLuaState, 1);
            integral::pushFunction(lambdaLuaState, [x](int y) -> int {
                return x*y;
            });
            return 1;
        });
        integral::setLuaFunction(luaState.get(), "getMultiplicationFunction4", [](lua_State *lambdaLuaState) -> int {
            integral::get<int>(lambdaLuaState, -1);
            integral::pushLuaFunction(lambdaLuaState, [](lua_State *lambdaLambdaLuaState) -> int {
                const int x = integral::get<int>(lambdaLambdaLuaState, integral::LuaFunctionWrapper::getUpValueIndex(1));
                const int y = integral::get<int>(lambdaLambdaLuaState, 1);
                integral::push<int>(lambdaLambdaLuaState, x*y);
                return 1;
            }, 1);
            return 1;
        });
        integral::setLuaFunction(luaState.get(), "getMultiplicationFunction5", [](lua_State *lambdaLuaState) -> int {
            const int x = integral::get<int>(lambdaLuaState, 1);
            integral::pushLuaFunction(lambdaLuaState, [x](lua_State *lambdaLambdaLuaState) -> int {
                const int y = integral::get<int>(lambdaLambdaLuaState, 1);
                integral::push<int>(lambdaLambdaLuaState, x*y);
                return 1;
            });
            return 1;
        });
        lua_setglobal(luaState.get(), "lib");
        REQUIRE_NOTHROW(stateView.doString("assert(lib.getSum(14, 28) == 28 + 14)"));
        REQUIRE_THROWS_AS(stateView.doString("lib.getSum('wrong argument type')"), integral::StateException);
        REQUIRE_THROWS_AS(stateView.doString("lib.getSum(1, 2, 3)"), integral::StateException);
        REQUIRE_NOTHROW(stateView.doString("assert(lib.getObjectId(lib.makeObject('id')) == 'id')"));
        REQUIRE_THROWS_AS(stateView.doString("lib.getObjectId('wrong argument type')"), integral::StateException);
        REQUIRE_NOTHROW(stateView.doString("assert(lib.getMultiplicationFunction1(21)(2) == 42)"));
        REQUIRE_NOTHROW(stateView.doString("assert(lib.getMultiplicationFunction2(21)(2) == 42)"));
        REQUIRE_NOTHROW(stateView.doString("assert(lib.getMultiplicationFunction3(21)(2) == 42)"));
        REQUIRE_NOTHROW(stateView.doString("assert(lib.getMultiplicationFunction4(21)(2) == 42)"));
    }
    SECTION("integral::pushClassMetatable, methods, constructor and integral::IgnoredArgument") {
        integral::pushClassMetatable<Object>(luaState.get());
        integral::setConstructor<Object(const std::string &)>(luaState.get(), "new1");
        integral::setConstructor<Object(unsigned)>(luaState.get(), "new2");
        integral::setFunction(luaState.get(), "getConstructor", []() -> integral::ConstructorWrapper<Object(const char *)> {
            return integral::ConstructorWrapper<Object(const char *)>();
        });
        integral::setFunction(luaState.get(), "getId", std::function<std::string(const Object &)>(&Object::getId));
        integral::setFunction(luaState.get(), "setId", &Object::setId);
        integral::setCopyGetter(luaState.get(), "getFlag", &Object::flag_);
        integral::setSetter(luaState.get(), "setFlag", &Object::flag_);
        integral::setFunction(luaState.get(), "hasSameId", [](const Object &object1, const Object &object2) -> bool {
            return object1.getId() == object2.getId();
        });
        integral::setFunction(luaState.get(), "__len", [](const Object &object, integral::LuaIgnoredArgument) {
            return object.getId().size();
        });
        integral::setFunction(luaState.get(), "throwException", []() -> void {
            throw std::runtime_error("C++ exception");
        });
        lua_setglobal(luaState.get(), "Object");
        REQUIRE_NOTHROW(stateView.doString("object = Object.new1('id')"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.new2(42):getId() == '42')"));
        REQUIRE_THROWS_AS(stateView.doString("Object.new2('string')"), integral::StateException);
        REQUIRE_NOTHROW(stateView.doString("assert(Object.getConstructor()('temporary'):getId() == 'temporary')"));
        REQUIRE_NOTHROW(stateView.doString("object:setId('newId'); assert(object:getId() == 'newId')"));
        REQUIRE_NOTHROW(stateView.doString("object:setFlag(true); assert(object:getFlag() == true)"));
        REQUIRE_NOTHROW(stateView.doString("assert(object:hasSameId(Object.new1('newId')) == true)"));
        REQUIRE_NOTHROW(stateView.doString("assert(#object == #object:getId())"));
        REQUIRE_THROWS_AS(stateView.doString("object.getId()"), integral::StateException);
        REQUIRE_THROWS_AS(stateView.doString("Object.throwException()"), integral::StateException);
    }
    SECTION("integral::detail::Reference integral::detail::Composite ClassMetatable, methods, functions and luaFunctions") {
        stateView["Object"].set(integral::ClassMetatable<Object>()
                                .set("new1", integral::ConstructorWrapper<Object(const std::string &)>())
                                .set("getId", integral::FunctionWrapper<std::string(const Object &)>(&Object::getId)));
        REQUIRE_NOTHROW(stateView.doString("object = Object.new1('id')"));
        REQUIRE_NOTHROW(stateView.doString("assert(object:getId() == 'id')"));
        stateView["Object"]["setId"].set(integral::FunctionWrapper<void(Object &, const std::string &)>(&Object::setId));
        REQUIRE_NOTHROW(stateView.doString("object:setId('42'); assert(object:getId() == '42')"));
        stateView["Object"]["aux"].set(integral::Table()
                                       .set("hasSameId", integral::FunctionWrapper<bool(const Object &, const Object &)>([](const Object &object1, const Object &object2) -> bool {
                                            return object1.getId() == object2.getId();
                                        }))
                                        .set("new", integral::LuaFunctionWrapper([](lua_State *lambdaLuaState) -> int {
                                            integral::push<Object>(lambdaLuaState, integral::get<unsigned>(lambdaLuaState, 1));
                                            return 1;
                                        })));
        REQUIRE_THROWS_AS(stateView.doString("Object.aux.new('string')"), integral::StateException);
        REQUIRE_NOTHROW(stateView.doString("object2 = Object.aux.new(42)"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.aux.hasSameId(object, object2) == true)"));
    }
    SECTION("integral::push and integral::get with Adaptors") {
        const std::vector<int> vector{1, 2, 3};
        integral::push<integral::LuaVector<int>>(luaState.get(), vector);
        REQUIRE((integral::get<integral::LuaVector<int>>(luaState.get(), -1) == vector));
        lua_pop(luaState.get(), 1);
        const std::vector<std::vector<Object>> vectorOfVectors{{Object("object[0][0]")}, {Object("object[1][0]"), Object("object[1][1]")}};
        const integral::LuaVector<integral::LuaVector<Object>> LuaVectorOfLuaVectors(vectorOfVectors.begin(), vectorOfVectors.end());
        integral::push<integral::LuaVector<integral::LuaVector<Object>>>(luaState.get(), LuaVectorOfLuaVectors);
        REQUIRE((integral::get<integral::LuaVector<integral::LuaVector<Object>>>(luaState.get(), -1) == LuaVectorOfLuaVectors));
        lua_pop(luaState.get(), 1);
        const std::array<double, 3> array{1.3, 2.2, 3.1};
        integral::push<integral::LuaArray<double, 3>>(luaState.get(), array);
        REQUIRE((integral::get<integral::LuaArray<double, 3>>(luaState.get(), -1) == array));
        lua_pop(luaState.get(), 1);
        const std::unordered_map<std::string, integral::LuaArray<Object, 2>> unorderedMapOfLuaArrays{{"a", std::array<Object, 2>{Object("objectA1"), Object("objectA2")}}, {"b", std::array<Object, 2>{Object("objectB1"), Object("objectB2")}}};
        integral::push<integral::LuaUnorderedMap<std::string, integral::LuaArray<Object, 2>>>(luaState.get(), unorderedMapOfLuaArrays);
        REQUIRE((integral::get<integral::LuaUnorderedMap<std::string, integral::LuaArray<Object, 2>>>(luaState.get(), -1) == unorderedMapOfLuaArrays));
        lua_pop(luaState.get(), 1);
        std::tuple<std::tuple<int, std::string>, Object> tuple(std::tuple<int, std::string>(42, "string"), Object("object"));
        integral::push<integral::LuaTuple<integral::LuaTuple<int, std::string>, Object>>(luaState.get(), tuple);
        REQUIRE((integral::get<integral::LuaTuple<integral::LuaTuple<int, std::string>, Object>>(luaState.get(), -1) == tuple));
        lua_pop(luaState.get(), 1);
    }
    SECTION("lua table to Reference") {
        REQUIRE_NOTHROW(stateView.doString("x = {42.1, y = {'a', [4.2] = 1}, [-42] = 'last'}"));
        REQUIRE(stateView["x"][1].get<double>() == 42.1);
        REQUIRE(stateView["x"]["y"][1].get<std::string>() == "a");
        REQUIRE(stateView["x"]["y"][4.2].get<unsigned>() == 1);
        REQUIRE(stateView["x"][-42].get<std::string>() == "last");
        REQUIRE_THROWS_AS(stateView["x"][2].get<int>(), integral::ReferenceException);
        REQUIRE_THROWS_AS(stateView["x"]["y"].get<std::string>(), integral::ReferenceException);
        REQUIRE_THROWS_AS(stateView["x"]["y"][1].get<int>(), integral::ReferenceException);
        const std::unordered_map<std::string, integral::LuaArray<Object, 2>> unorderedMapOfLuaArrays{{"a", std::array<Object, 2>{Object("objectA1"), Object("objectA2")}}, {"b", std::array<Object, 2>{Object("objectB1"), Object("objectB2")}}};
        stateView["y"].set<integral::LuaUnorderedMap<std::string, integral::LuaArray<Object, 2>>>(unorderedMapOfLuaArrays);
        REQUIRE(stateView["y"]["a"][1].get<Object>() == Object("objectA1"));
        REQUIRE(stateView["y"]["a"][2].get<Object>() == Object("objectA2"));
        REQUIRE(stateView["y"]["b"][1].get<Object>() == Object("objectB1"));
        REQUIRE(stateView["y"]["b"][2].get<Object>() == Object("objectB2"));
    }
    SECTION("lua table to Adaptors") {
        REQUIRE_NOTHROW(stateView.doString("x = {11, 22, 33}"));
        // TODO with Object interaction
    }
    REQUIRE(lua_gettop(luaState.get()) == 0);
}
