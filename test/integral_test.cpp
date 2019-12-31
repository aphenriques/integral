//
//  integral_test.cpp
//  integral
//
//  Copyright (C) 2017, 2019  André Pereira Henriques
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

#include <cmath>
#include <cstring>
#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <catch.hpp>
#include <integral/integral.hpp>
#include <exception/Exception.hpp>

template<typename T>
using VectorAdaptor = integral::Adaptor<std::vector<T>>;

class BaseOfInnerObject {
public:
    int getBaseOfInnerConstant() const  {
        return 21;
    }
};

class InnerObject : public BaseOfInnerObject {
public:
    std::string getGreeting() const {
        return "hello";
    }
};

class BaseOfBaseObject {
public:
    std::string getBaseOfBaseString() const {
        return "BaseOfBase";
    }
};

class BaseObject : public BaseOfBaseObject {
public:
    int getBaseConstant() const {
        return 42;
    }
};

class Object : public BaseObject {
public:
    bool flag_ = false;
    InnerObject innerObject_;

    Object() = default;
    Object(const std::string &id) : id_(id) {}
    Object(unsigned id) : id_(std::to_string(id)) {}
    Object(const std::string &prefix, unsigned id) : id_(prefix + std::to_string(id)) {}

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
        integral::push<std::optional<Object>>(luaState.get(), Object("object"));
        integral::push<std::optional<Object>>(luaState.get(), std::nullopt);
        REQUIRE(integral::get<int>(luaState.get(), -11) == -42);
        REQUIRE(integral::get<int>(luaState.get(), 1) == -42);
        REQUIRE(integral::get<unsigned>(luaState.get(), -10) == 42u);
        REQUIRE(integral::get<unsigned>(luaState.get(), 2) == 42u);
        REQUIRE(integral::get<double>(luaState.get(), -9) == 42.1);
        REQUIRE(integral::get<double>(luaState.get(), 3) == 42.1);
        REQUIRE(std::strcmp(integral::get<const char *>(luaState.get(), -8), "string1") == 0);
        REQUIRE(std::strcmp(integral::get<const char *>(luaState.get(), 4), "string1") == 0);
        REQUIRE(integral::get<std::string>(luaState.get(), -8) == "string1");
        REQUIRE(integral::get<std::string>(luaState.get(), -7) == "string2");
        REQUIRE(integral::get<std::string>(luaState.get(), 5) == "string2");
        REQUIRE(std::strcmp(integral::get<const char *>(luaState.get(), -7), "string2") == 0);
        REQUIRE(integral::get<std::string>(luaState.get(), -6) == stringWithNullCharacter);
        REQUIRE(integral::get<std::string>(luaState.get(), 6) == stringWithNullCharacter);
        REQUIRE(integral::get<bool>(luaState.get(), -5) == true);
        REQUIRE(integral::get<bool>(luaState.get(), 7) == true);
        REQUIRE(integral::get<bool>(luaState.get(), -4) == false);
        REQUIRE(integral::get<bool>(luaState.get(), 8) == false);
        REQUIRE(integral::get<Object>(luaState.get(), -3).getId() == "object");
        REQUIRE(integral::get<Object>(luaState.get(), 9).getId() == "object");
        REQUIRE(integral::get<std::optional<Object>>(luaState.get(), -2).value().getId() == "object");
        REQUIRE(integral::get<std::optional<Object>>(luaState.get(), 10).value().getId() == "object");
        REQUIRE(integral::get<std::optional<Object>>(luaState.get(), -1) == std::nullopt);
        REQUIRE(integral::get<std::optional<Object>>(luaState.get(), 11) == std::nullopt);
        lua_pop(luaState.get(), 11);
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
    SECTION("integral::StateView::StateView(integral::StateView &&) and integral::StateView::operator=(integral::StateView &&)") {
        REQUIRE_NOTHROW(stateView.doString("x = 42"));
        integral::StateView testStateView(integral::StateView(luaState.get()));
        REQUIRE_NOTHROW(testStateView.doString("assert(x == 42)"));
        integral::State testState;
        testState.openLibs();
        testStateView = integral::StateView(testState.getLuaState());
        REQUIRE_NOTHROW(testStateView.doString("assert(x == nil)"));
    }
    SECTION("integral::detail::Reference::emplace and integral::detail::Reference::get") {
        stateView["x"].emplace<Object>("object");
        REQUIRE(stateView["x"].get<Object>() == Object("object"));
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
    SECTION("integral::detail::Reference::isNil") {
        REQUIRE(stateView["x"].isNil() == true);
        stateView["x"] = integral::Table();
        REQUIRE(stateView["x"].isNil() == false);
        REQUIRE(stateView["x"][1].isNil() == true);
        stateView["x"][1] = 42;
        REQUIRE(stateView["x"][1].isNil() == false);
    }
    SECTION("integral::detail::Reference::get incompatible types") {
        stateView["x"].set("string");
        REQUIRE_THROWS_AS(stateView["x"].get<int>(), integral::ReferenceException);
        REQUIRE_THROWS_AS(stateView["x"].get<Object>(), integral::ReferenceException);
        stateView["x"].set(Object("object"));
        REQUIRE_THROWS_AS(stateView["x"].get<int>(), integral::ReferenceException);
        REQUIRE_THROWS_AS(stateView["x"].get<std::string>(), integral::ReferenceException);
    }
    SECTION("integral::detail::Reference::operator= and integral::detail::Reference conversion operator") {
        stateView["x"] = -42;
        int xInt = stateView["x"];
        REQUIRE(xInt == -42);
        stateView["x"] = 42u;
        unsigned xUnsigned = stateView["x"];
        REQUIRE(xUnsigned == 42u);
        stateView["x"] = 42.1;
        double xDouble = stateView["x"];
        REQUIRE(xDouble == 42.1);
        stateView["x"] = "string1";
        const char * xCString = stateView["x"];
        REQUIRE(std::strcmp(xCString, "string1") == 0);
        stateView["x"] = std::string("string2");
        std::string xString = stateView["x"];
        REQUIRE(stateView["x"].get<std::string>() == "string2");
        const std::string stringWithNullCharacter("string\0with null", 17);
        stateView["x"] = stringWithNullCharacter;
        std::string xStringWithNull = stateView["x"];
        REQUIRE(xStringWithNull == stringWithNullCharacter);
        stateView["x"] = true;
        bool xBool = stateView["x"];
        REQUIRE(xBool == true);
        stateView["x"] = false;
        xBool = stateView["x"];
        REQUIRE(xBool == false);
        stateView["x"] = Object("object");
        Object xObject = stateView["x"];
        REQUIRE(xObject == Object("object"));
    }
    SECTION("integral::detail::Reference conversion operator with incompatible types") {
        stateView["x"] = "string";
        REQUIRE_THROWS_AS([&stateView]() -> void {
            int x = stateView["x"];
        }(), integral::ReferenceException);
        REQUIRE_THROWS_AS([&stateView]() -> void {
            Object x = stateView["x"];
        }(), integral::ReferenceException);
        stateView["x"] = Object("object");
        REQUIRE_THROWS_AS([&stateView]() -> void {
            int x = stateView["x"];
        }(), integral::ReferenceException);
        REQUIRE_THROWS_AS([&stateView]() -> void {
            std::string x = stateView["x"];
        }(), integral::ReferenceException);
    }
    SECTION("integral::setFunction and integral::setLuaFunction") {
        lua_newtable(luaState.get());
        integral::setFunction(luaState.get(), "getSum", getSum);
        integral::setFunction(luaState.get(), "makeObject", makeObject);
        integral::setFunction(luaState.get(), "getObjectId", &Object::getId);
        integral::setFunction(luaState.get(), "getMultiplicationFunction1", [](int x) -> integral::FunctionWrapper<int(int)> {
            return integral::makeFunctionWrapper([x](int y) -> int {
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
        integral::setFunction(luaState.get(), "getId", &Object::getId);
        integral::setFunction(luaState.get(), "setId", &Object::setId);
        integral::setGetter(luaState.get(), "getFlag", &Object::flag_);
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
    SECTION("TableComposite, ClassMetatableComposite ClassMetatable, methods, functions and luaFunctions") {
        stateView["Object"].set(integral::ClassMetatable<Object>()
                                .set("new1", integral::ConstructorWrapper<Object(const std::string &)>())
                                .setConstructor<Object(unsigned)>("new2")
                                .set("getId", integral::FunctionWrapper<std::string(const Object &)>(&Object::getId))
                                .setGetter("getFlag", &Object::flag_)
                                .setSetter("setFlag", &Object::flag_)
                                .setLuaFunction("getIdAndFlag", [](lua_State *lambdaLuaState) -> int {
                                    const Object &object = integral::get<Object>(lambdaLuaState, 1);
                                    integral::push<const char *>(lambdaLuaState, object.getId().c_str());
                                    integral::push<bool>(lambdaLuaState, object.flag_);
                                    return 2;
                                })
                                .emplace<Object>("sample", "#", 42u)
                                );
        REQUIRE_NOTHROW(stateView.doString("object = Object.new1('id')"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.new2(42):getId() == '42')"));
        REQUIRE_NOTHROW(stateView.doString("assert(object:getId() == 'id')"));
        REQUIRE_NOTHROW(stateView.doString("assert(object:getFlag() == false)"));
        REQUIRE_NOTHROW(stateView.doString("object:setFlag(true)"));
        REQUIRE_NOTHROW(stateView.doString("assert(object:getFlag() == true)"));
        REQUIRE_NOTHROW(stateView.doString("id, flag = object:getIdAndFlag(); assert(id == 'id' and flag == true)"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.sample:getId() == '#42')"));
        stateView["Object"]["setId"].setFunction(&Object::setId);
        REQUIRE_NOTHROW(stateView.doString("object:setId('42'); assert(object:getId() == '42')"));
        stateView["Object"]["aux"].set(integral::Table()
                                       .setFunction("hasSameId", [](const Object &object1, const Object &object2) -> bool {
                                           return object1.getId() == object2.getId();
                                       })
                                       .set("new", integral::LuaFunctionWrapper([](lua_State *lambdaLuaState) -> int {
                                           integral::push<Object>(lambdaLuaState, integral::get<unsigned>(lambdaLuaState, 1));
                                           return 1;
                                       }))
                                       .setLuaFunction("new2", [](lua_State *lambdaLuaState) -> int {
                                           integral::push<Object>(lambdaLuaState, 10u);
                                           return 1;
                                       })
                                        .emplace<Object>("sample", "#", 42u)
                                       );
        REQUIRE_THROWS_AS(stateView.doString("Object.aux.new('string')"), integral::StateException);
        REQUIRE_NOTHROW(stateView.doString("assert(Object.aux.new2():getId() == '10')"));
        REQUIRE_NOTHROW(stateView.doString("object2 = Object.aux.new(42)"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.aux.hasSameId(object, object2) == true)"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.aux.sample:getId() == '#42')"));
        stateView["Object"]["aux"]["get42"].setLuaFunction([](lua_State *lambdaLuaState) -> int {
            integral::push<int>(lambdaLuaState, 42);
            return 1;
        });
        REQUIRE_NOTHROW(stateView.doString("assert(Object.aux.get42() == 42)"));
    }
    SECTION("table conversion with integral::push and integral::get") {
        const std::vector<int> vector{1, 2, 3};
        integral::push<std::vector<int>>(luaState.get(), vector);
        REQUIRE((integral::get<std::vector<int>>(luaState.get(), -1) == vector));
        lua_pop(luaState.get(), 1);
        const std::vector<std::vector<Object>> vectorOfVectors = {{Object("object[0][0]")}, {Object("object[1][0]"), Object("object[1][1]")}};
        integral::push<std::vector<std::vector<Object>>>(luaState.get(), vectorOfVectors);
        REQUIRE((integral::get<std::vector<std::vector<Object>>>(luaState.get(), -1) == vectorOfVectors));
        lua_pop(luaState.get(), 1);
        const std::array<double, 3> array{1.3, 2.2, 3.1};
        integral::push<std::array<double, 3>>(luaState.get(), array);
        REQUIRE((integral::get<std::array<double, 3>>(luaState.get(), -1) == array));
        lua_pop(luaState.get(), 1);
        const std::unordered_map<std::string, std::array<Object, 2>> unorderedMapOfArrays = {{"a", {Object("objectA1"), Object("objectA2")}}, {"b", {Object("objectB1"), Object("objectB2")}}};
        integral::push<std::unordered_map<std::string, std::array<Object, 2>>>(luaState.get(), unorderedMapOfArrays);
        REQUIRE((integral::get<std::unordered_map<std::string, std::array<Object, 2>>>(luaState.get(), -1) == unorderedMapOfArrays));
        lua_pop(luaState.get(), 1);
        std::tuple<std::tuple<int, std::string>, Object> tuple{{42, "string"}, Object("object")};
        integral::push<std::tuple<std::tuple<int, std::string>, Object>>(luaState.get(), tuple);
        REQUIRE((integral::get<std::tuple<std::tuple<int, std::string>, Object>>(luaState.get(), -1) == tuple));
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
        std::unordered_map<std::string, std::array<Object, 2>> unorderedMapOfArrays = {{"a", std::array<Object, 2>{Object("objectA1"), Object("objectA2")}}, {"b", std::array<Object, 2>{Object("objectB1"), Object("objectB2")}}};
        stateView["y"].set(unorderedMapOfArrays);
        REQUIRE(stateView["y"]["a"][1].get<Object>() == Object("objectA1"));
        REQUIRE(stateView["y"]["a"][2].get<Object>() == Object("objectA2"));
        REQUIRE(stateView["y"]["b"][1].get<Object>() == Object("objectB1"));
        REQUIRE(stateView["y"]["b"][2].get<Object>() == Object("objectB2"));
    }
    SECTION("lua table to stl types") {
        REQUIRE_NOTHROW(stateView.doString("x = {11, 22, 33}"));
        lua_getglobal(luaState.get(), "x");
        REQUIRE((integral::get<std::array<int, 3>>(luaState.get(), -1) == std::array<int, 3>{11, 22, 33}));
        lua_pop(luaState.get(), 1);
        REQUIRE((stateView["x"].get<std::array<int, 3>>() == std::array<int, 3>{11, 22, 33}));
        stateView["Object"].set(integral::ClassMetatable<Object>()
                                .set("new", integral::ConstructorWrapper<Object(const std::string &)>()));
        REQUIRE_NOTHROW(stateView.doString("y = {Object.new('o0'), {o1 = Object.new('o1'), o2 = Object.new('o2')}}"));
        lua_getglobal(luaState.get(), "y");
        REQUIRE(std::get<0>(integral::get<std::tuple<Object, std::unordered_map<std::string, Object>>>(luaState.get(), -1)).getId() == "o0");
        REQUIRE(std::get<1>(integral::get<std::tuple<Object, std::unordered_map<std::string, Object>>>(luaState.get(), -1))["o1"].getId() == "o1");
        REQUIRE(std::get<1>(integral::get<std::tuple<Object, std::unordered_map<std::string, Object>>>(luaState.get(), -1))["o2"].getId() == "o2");
        lua_pop(luaState.get(), 1);
        REQUIRE(std::get<0>(stateView["y"].get<std::tuple<Object, std::unordered_map<std::string, Object>>>()).getId() == "o0");
        REQUIRE(std::get<1>(stateView["y"].get<std::tuple<Object, std::unordered_map<std::string, Object>>>())["o1"].getId() == "o1");
        REQUIRE(std::get<1>(stateView["y"].get<std::tuple<Object, std::unordered_map<std::string, Object>>>())["o2"].getId() == "o2");
        // numbers are converted to string by lua
        REQUIRE_NOTHROW(stateView.doString("z = {{Object.new(11), Object.new(12)}, {Object.new(21), Object.new(22)}}"));
        lua_getglobal(luaState.get(), "z");
        REQUIRE(integral::get<std::vector<std::vector<Object>>>(luaState.get(), -1).at(0).at(0).getId() == "11");
        REQUIRE(integral::get<std::vector<std::vector<Object>>>(luaState.get(), -1).at(0).at(1).getId() == "12");
        REQUIRE(integral::get<std::vector<std::vector<Object>>>(luaState.get(), -1).at(1).at(0).getId() == "21");
        REQUIRE(integral::get<std::vector<std::vector<Object>>>(luaState.get(), -1).at(1).at(1).getId() == "22");
        REQUIRE((integral::get<std::array<std::array<Object, 2>, 2>>(luaState.get(), -1)[0][0].getId() == "11"));
        REQUIRE((integral::get<std::array<std::array<Object, 2>, 2>>(luaState.get(), -1)[0][1].getId() == "12"));
        REQUIRE((integral::get<std::array<std::array<Object, 2>, 2>>(luaState.get(), -1)[1][0].getId() == "21"));
        REQUIRE((integral::get<std::array<std::array<Object, 2>, 2>>(luaState.get(), -1)[1][1].getId() == "22"));
        REQUIRE((integral::get<std::array<std::vector<Object>, 2>>(luaState.get(), -1)[0].at(0).getId() == "11"));
        REQUIRE((integral::get<std::array<std::vector<Object>, 2>>(luaState.get(), -1)[0].at(1).getId() == "12"));
        REQUIRE((integral::get<std::array<std::vector<Object>, 2>>(luaState.get(), -1)[1].at(0).getId() == "21"));
        REQUIRE((integral::get<std::array<std::vector<Object>, 2>>(luaState.get(), -1)[1].at(1).getId() == "22"));
        lua_pop(luaState.get(), 1);
        REQUIRE(stateView["z"].get<std::vector<std::vector<Object>>>().at(0).at(0).getId() == "11");
        REQUIRE(stateView["z"].get<std::vector<std::vector<Object>>>().at(0).at(1).getId() == "12");
        REQUIRE(stateView["z"].get<std::vector<std::vector<Object>>>().at(1).at(0).getId() == "21");
        REQUIRE(stateView["z"].get<std::vector<std::vector<Object>>>().at(1).at(1).getId() == "22");
        REQUIRE((stateView["z"].get<std::array<std::array<Object, 2>, 2>>()[0][0].getId() == "11"));
        REQUIRE((stateView["z"].get<std::array<std::array<Object, 2>, 2>>()[0][1].getId() == "12"));
        REQUIRE((stateView["z"].get<std::array<std::array<Object, 2>, 2>>()[1][0].getId() == "21"));
        REQUIRE((stateView["z"].get<std::array<std::array<Object, 2>, 2>>()[1][1].getId() == "22"));
        REQUIRE((stateView["z"].get<std::array<std::vector<Object>, 2>>()[0].at(0).getId() == "11"));
        REQUIRE((stateView["z"].get<std::array<std::vector<Object>, 2>>()[0].at(1).getId() == "12"));
        REQUIRE((stateView["z"].get<std::array<std::vector<Object>, 2>>()[1].at(0).getId() == "21"));
        REQUIRE((stateView["z"].get<std::array<std::vector<Object>, 2>>()[1].at(1).getId() == "22"));
    }
    SECTION("std::optional conversion") {
        stateView["x"] = std::optional<Object>(std::nullopt);
        REQUIRE(stateView["x"].get<std::optional<Object>>().has_value() == false);
        stateView["x"] = std::optional<Object>("object");
        REQUIRE(stateView["x"].get<std::optional<Object>>().value().getId() == "object");
        REQUIRE_NOTHROW(stateView.doString("function f(x) if x == nil then return 42 else return nil end end"));
        REQUIRE_NOTHROW(stateView["f"].call<std::optional<int>>(1).has_value() == false);
        REQUIRE_NOTHROW(stateView["f"].call<std::optional<int>>(std::optional<int>(std::nullopt)).value() == 42);
        stateView["g"].setFunction([](std::optional<int> x) {
            if (x.has_value() == false) {
                return std::optional<int>(42);
            } else {
                return std::optional<int>(std::nullopt);
            }
        });
        REQUIRE_NOTHROW(stateView.doString("assert(g(1) == nil)"));
        REQUIRE_NOTHROW(stateView.doString("assert(g(nil) == 42)"));
        stateView.doString("x = nil");
        std::optional<Object> optionalX;
        // the following expression throws because of an obscure c++ behaviour
        // see https://stackoverflow.com/a/45865802
        REQUIRE_THROWS_AS(optionalX = stateView["x"], integral::ReferenceException);
    }
    SECTION("function call") {
        stateView["Object"].set(integral::ClassMetatable<Object>()
                                .setConstructor<Object(const std::string &)>("new")
                                );
        REQUIRE_NOTHROW(stateView.doString("function makeObject(id) return Object.new(id) end"));
        REQUIRE_NOTHROW(stateView.doString("function dummy() end"));
        REQUIRE_NOTHROW(stateView.doString("x = 'not function'"));
        lua_getglobal(luaState.get(), "makeObject");
        lua_pushvalue(luaState.get(), -1);
        lua_pushvalue(luaState.get(), -1);
        lua_pushvalue(luaState.get(), -1);
        lua_pushvalue(luaState.get(), -1);
        REQUIRE_NOTHROW(integral::call<Object>(luaState.get(), "id42"));
        REQUIRE(integral::call<Object>(luaState.get(), "id42").getId() == "id42");
        REQUIRE_THROWS_AS(integral::call<Object>(luaState.get()), integral::CallerException);
        REQUIRE_THROWS_AS(integral::call<Object>(luaState.get(), Object(42)), integral::CallerException);
        REQUIRE_THROWS_AS(integral::call<int>(luaState.get(), "id"), integral::ArgumentException);
        lua_getglobal(luaState.get(), "dummy");
        lua_pushvalue(luaState.get(), -1);
        REQUIRE_NOTHROW(integral::call<void>(luaState.get()));
        REQUIRE_THROWS_AS(integral::call<int>(luaState.get()), integral::ArgumentException);
        lua_getglobal(luaState.get(), "x");
        REQUIRE_THROWS_AS(integral::call<void>(luaState.get()), integral::CallerException);
        REQUIRE(stateView["makeObject"].call<Object>("id-42").getId() == "id-42");
        REQUIRE_THROWS_AS(stateView["makeObject"].call<int>("id-42"), integral::ReferenceException);
        REQUIRE_NOTHROW(stateView["dummy"].call<void>());
        REQUIRE_THROWS_AS(stateView["x"].call<void>(), integral::ReferenceException);
    }
    SECTION("LuaIgnoredArgument and LuaFunctionArgument") {
        stateView["Vector"].set(integral::ClassMetatable<VectorAdaptor<double>>()
                                .setConstructor<VectorAdaptor<double>(const std::vector<double> &)>("new")
                                .setFunction("__len", [](const VectorAdaptor<double> &vector, integral::LuaIgnoredArgument) -> std::size_t {
                                    return vector.size();
                                })
                                .setFunction("transform", [](VectorAdaptor<double> &vector, const integral::LuaFunctionArgument &luaFunctionArgument) -> void {
                                    std::for_each(vector.begin(), vector.end(), [&luaFunctionArgument](double &element) -> void {
                                        element = luaFunctionArgument.call<double>(element);
                                    });
                                })
                                );
        REQUIRE_NOTHROW(stateView.doString("vector = Vector.new({1.1, 1.2, 1.3})"));
        REQUIRE_NOTHROW(stateView.doString("assert(#vector == 3)"));
        REQUIRE_THROWS_AS(stateView.doString("assert(#vector == 4)"), integral::StateException);
        REQUIRE_NOTHROW(stateView.doString("function getNegative(x) return -1*x end"));
        REQUIRE_NOTHROW(stateView.doString("vector:transform(getNegative)"));
        REQUIRE_NOTHROW(stateView["vector"].get<VectorAdaptor<double>>());
        REQUIRE(stateView["vector"].get<VectorAdaptor<double>>().size() == 3);
        REQUIRE(stateView["vector"].get<VectorAdaptor<double>>().at(0) < 0);
        REQUIRE(stateView["vector"].get<VectorAdaptor<double>>().at(1) < 0);
        REQUIRE(stateView["vector"].get<VectorAdaptor<double>>().at(2) < 0);
        stateView["getAbs"].set(integral::makeFunctionWrapper(static_cast<double(*)(double)>(&std::abs)));
        REQUIRE_NOTHROW(stateView.doString("vector:transform(getAbs)"));
        REQUIRE(stateView["vector"].get<VectorAdaptor<double>>().at(0) > 0);
        REQUIRE(stateView["vector"].get<VectorAdaptor<double>>().at(1) > 0);
        REQUIRE(stateView["vector"].get<VectorAdaptor<double>>().at(2) > 0);
    }
    SECTION("polymorphism") {
        stateView["Object"].set(integral::ClassMetatable<Object>()
                                .setConstructor<Object(const char *)>("new")
                                );
        stateView["getBaseConstant"].setFunction(&BaseObject::getBaseConstant);
        stateView["getBaseOfBaseString"].setFunction(&BaseOfBaseObject::getBaseOfBaseString);
        REQUIRE_NOTHROW(stateView.doString("object = Object.new(21)"));
        REQUIRE_NOTHROW(stateView.doString("assert(getBaseConstant(object) == 42)"));
        REQUIRE_NOTHROW(stateView.doString("assert(getBaseOfBaseString(object) == 'BaseOfBase')"));
    }
    SECTION("core default argument") {
        integral::pushClassMetatable<Object>(luaState.get());
        integral::setConstructor<Object(const std::string &)>(luaState.get(), "new", integral::DefaultArgument<std::string, 1>("default"));
        integral::setFunction(luaState.get(), "getId", &Object::getId);
        integral::setFunction(luaState.get(), "getBaseConstant", &BaseObject::getBaseConstant, integral::DefaultArgument<BaseObject, 1>());
        integral::setFunction(luaState.get(), "getSum", &getSum, integral::DefaultArgument<double, 2>(1));
        integral::setFunction(luaState.get(), "testDefault", [](int number, const std::string &string) -> std::string {
            return std::to_string(number) + string;
        }, integral::DefaultArgument<int, 1>(42), integral::DefaultArgument<std::string, 2>("_"));
        lua_setglobal(luaState.get(), "Object");
        REQUIRE_NOTHROW(stateView.doString("assert(Object.new():getId() == 'default')"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.getBaseConstant() == 42)"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.getSum(42) > 42)"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.testDefault() == '42_')"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.testDefault(421) == '421_')"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.testDefault(nil, '#') == '42#')"));
    }
    SECTION("Reference default argument") {
        stateView["Object"].set(integral::ClassMetatable<Object>()
                                .setConstructor<Object(const std::string &)>("new", integral::DefaultArgument<std::string, 1>("default"))
                                .setFunction("getId", &Object::getId)
                                .setFunction("getBaseConstant", &BaseObject::getBaseConstant, integral::DefaultArgument<BaseObject, 1>())
                                .setFunction("getSum", &getSum, integral::DefaultArgument<double, 2>(1))
                                .setFunction("testDefault", [](int number, const std::string &string) -> std::string {
                                    return std::to_string(number) + string;
                                }, integral::DefaultArgument<int, 1>(42), integral::DefaultArgument<std::string, 2>("_"))
                                );
        REQUIRE_NOTHROW(stateView.doString("assert(Object.new():getId() == 'default')"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.getBaseConstant() == 42)"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.getSum(42) > 42)"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.testDefault() == '42_')"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.testDefault(421) == '421_')"));
        REQUIRE_NOTHROW(stateView.doString("assert(Object.testDefault(nil, '#') == '42#')"));
    }
    SECTION("inheritance and type convertion") {
        stateView["BaseOfBaseObject"].set(integral::ClassMetatable<BaseOfBaseObject>()
                                          .setConstructor<BaseOfBaseObject()>("new")
                                          .setFunction("getBaseOfBaseString", &BaseOfBaseObject::getBaseOfBaseString)
                                          );
        REQUIRE_NOTHROW(stateView.doString("baseOfBase = BaseOfBaseObject.new()"));
        REQUIRE_NOTHROW(stateView.doString("assert(baseOfBase:getBaseOfBaseString() == 'BaseOfBase')"));
        stateView["BaseObject"].set(integral::ClassMetatable<BaseObject>()
                                    .setConstructor<BaseObject()>("new")
                                    .setFunction("getBaseConstant", &BaseObject::getBaseConstant)
                                    );
        stateView.defineInheritance<BaseObject, BaseOfBaseObject>();
        REQUIRE_NOTHROW(stateView.doString("base = BaseObject.new()"));
        REQUIRE_NOTHROW(stateView.doString("assert(base:getBaseConstant() == 42)"));
        REQUIRE_NOTHROW(stateView.doString("assert(base:getBaseOfBaseString() == 'BaseOfBase')"));
        stateView["Object"].set(integral::ClassMetatable<Object>()
                                .setConstructor<Object(unsigned)>("new")
                                .setFunction("getId", &Object::getId)
                                );
        stateView.defineInheritance<Object, BaseObject>();
        REQUIRE_NOTHROW(stateView.doString("object = Object.new(21)"));
        REQUIRE_NOTHROW(stateView.doString("assert(object:getId() == '21')"));
        REQUIRE_NOTHROW(stateView.doString("assert(object:getBaseConstant() == 42)"));
        REQUIRE_NOTHROW(stateView.doString("assert(object:getBaseOfBaseString() == 'BaseOfBase')"));
        Object cppObject("cppObject");
        stateView["cppObject"].set(std::ref(cppObject));
        REQUIRE_NOTHROW(stateView.doString("assert(cppObject:getId() == 'cppObject')"));
        REQUIRE_NOTHROW(stateView.doString("assert(cppObject:getBaseOfBaseString() == 'BaseOfBase')"));
        REQUIRE_NOTHROW(&stateView["cppObject"].get<Object>() == &cppObject);
        stateView["BaseOfInnerObject"].set(integral::ClassMetatable<BaseOfInnerObject>()
                                           .setFunction("getBaseOfInnerConstant", &BaseOfInnerObject::getBaseOfInnerConstant)
                                           );
        stateView["InnerObject"].set(integral::ClassMetatable<InnerObject>()
                                     .setFunction("getGreeting", &InnerObject::getGreeting)
                                     );
        stateView.defineInheritance<InnerObject, BaseOfInnerObject>();
        REQUIRE_THROWS_AS(stateView.doString("assert(InnerObject.getGreeting(cppObject) == 'hello')"), integral::StateException);
        REQUIRE_THROWS_AS(stateView.doString("cppObject:getGreeting()"), integral::StateException);
        REQUIRE_THROWS_AS(stateView.doString("cppObject:getBaseOfInnerConstant()"), integral::StateException);
        stateView.defineInheritance([](Object *object) -> InnerObject * {
            return &object->innerObject_;
        });
        REQUIRE_NOTHROW(stateView.doString("assert(cppObject:getGreeting() == 'hello')"));
        REQUIRE_NOTHROW(stateView.doString("assert(cppObject:getBaseOfInnerConstant() == 21)"));
        stateView["sharedObject"].set(std::make_shared<Object>("shared"));
        REQUIRE_NOTHROW(stateView.doString("assert(sharedObject:getId() == 'shared')"));
    }
    SECTION("ClassMetatable inheritance") {
        stateView["BaseOfInnerObject"].set(integral::ClassMetatable<BaseOfInnerObject>()
                                           .setFunction("getBaseOfInnerConstant", &BaseOfInnerObject::getBaseOfInnerConstant)
                                           );
        stateView["InnerObject"].set(integral::ClassMetatable<InnerObject>()
                                     .setFunction("getGreeting", &InnerObject::getGreeting)
                                     .setBaseClass<BaseOfInnerObject>()
                                     );
        stateView["BaseOfBaseObject"].set(integral::ClassMetatable<BaseOfBaseObject>()
                                          .setConstructor<BaseOfBaseObject()>("new")
                                          .setFunction("getBaseOfBaseString", &BaseOfBaseObject::getBaseOfBaseString)
                                          );
        REQUIRE_NOTHROW(stateView.doString("baseOfBase = BaseOfBaseObject.new()"));
        REQUIRE_NOTHROW(stateView.doString("assert(baseOfBase:getBaseOfBaseString() == 'BaseOfBase')"));
        stateView["BaseObject"].set(integral::ClassMetatable<BaseObject>()
                                    .setConstructor<BaseObject()>("new")
                                    .setFunction("getBaseConstant", &BaseObject::getBaseConstant)
                                    .setBaseClass<BaseOfBaseObject>()
                                    );
        REQUIRE_NOTHROW(stateView.doString("base = BaseObject.new()"));
        REQUIRE_NOTHROW(stateView.doString("assert(base:getBaseConstant() == 42)"));
        REQUIRE_NOTHROW(stateView.doString("assert(base:getBaseOfBaseString() == 'BaseOfBase')"));
        stateView["Object"].set(integral::ClassMetatable<Object>()
                                .setBaseClass<BaseObject>()
                                .setBaseClass([](Object *object) -> InnerObject * {
                                    return &object->innerObject_;
                                })
                                .setConstructor<Object(unsigned)>("new")
                                .set("getId", integral::FunctionWrapper<std::string(const Object &)>(&Object::getId))
                                );
        REQUIRE_NOTHROW(stateView.doString("object = Object.new(21)"));
        REQUIRE_NOTHROW(stateView.doString("assert(object:getId() == '21')"));
        REQUIRE_NOTHROW(stateView.doString("assert(object:getBaseConstant() == 42)"));
        REQUIRE_NOTHROW(stateView.doString("assert(object:getBaseOfBaseString() == 'BaseOfBase')"));
        REQUIRE_NOTHROW(stateView.doString("assert(object:getGreeting() == 'hello')"));
        REQUIRE_NOTHROW(stateView.doString("assert(object:getBaseOfInnerConstant() == 21)"));
        Object cppObject("cppObject");
        stateView["cppObject"].set(std::ref(cppObject));
        REQUIRE_NOTHROW(stateView.doString("assert(cppObject:getId() == 'cppObject')"));
        REQUIRE_NOTHROW(stateView.doString("assert(cppObject:getBaseOfBaseString() == 'BaseOfBase')"));
        REQUIRE_NOTHROW(&stateView["cppObject"].get<Object>() == &cppObject);
        REQUIRE_NOTHROW(stateView.doString("assert(cppObject:getGreeting() == 'hello')"));
        REQUIRE_NOTHROW(stateView.doString("assert(cppObject:getBaseOfInnerConstant() == 21)"));
        stateView["sharedObject"].set(std::make_shared<Object>("shared"));
        REQUIRE_NOTHROW(stateView.doString("assert(sharedObject:getId() == 'shared')"));
    }
    SECTION("Duplicate inheritance and type function definition exception test") {
        stateView["BaseOfBaseObject"].set(integral::ClassMetatable<BaseOfBaseObject>());
        stateView["BaseObject"].set(integral::ClassMetatable<BaseObject>());
        stateView.defineInheritance<BaseObject, BaseOfBaseObject>();
        stateView["Object"].set(integral::ClassMetatable<Object>());
        stateView.defineInheritance<Object, BaseObject>();
        // not a redefinition:
        REQUIRE_NOTHROW(stateView.defineInheritance<Object, BaseOfBaseObject>());
        REQUIRE_THROWS_AS((stateView.defineInheritance<Object, BaseObject>()), exception::LogicException);
        // this exception leaves the stack with trash
        lua_settop(stateView.getLuaState(), 0);
        REQUIRE_THROWS_AS((stateView.defineTypeFunction<Object, BaseObject>()), exception::LogicException);
        // this exception leaves the stack with trash
        lua_settop(stateView.getLuaState(), 0);
    }
    SECTION("Global") {
        stateView["x"] = 42;
        stateView["global"] = integral::Global();
        REQUIRE_NOTHROW(stateView.doString("assert(global.x == x)"));
        stateView["global"]["y"] = integral::Global()["x"];
        REQUIRE_NOTHROW(stateView.doString("assert(global.x == y)"));
    }
    REQUIRE(lua_gettop(luaState.get()) == 0);
}
