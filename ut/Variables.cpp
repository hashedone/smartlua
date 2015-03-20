/*
 * This file is part of SmartLua.
 *
 * SmartLua is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SmartLua is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * copyrigt Bartłomiej Kuras, 2015
 */

#include "../Lua.hpp"

#include <gtest/gtest.h>

#include <vector>

using namespace smartlua;

TEST(VariablesTest, Getting)
{
	Lua sut;

	sut.script("x = true");
	EXPECT_TRUE(sut.global<bool>("x"));

	sut.script("x = 1.5");
	EXPECT_EQ(1.5, sut.global<float>("x"));

	sut.script("x = 1");
	EXPECT_EQ(1, sut.global<int>("x"));

	sut.script("x = { 1, 2, 3 }");
	EXPECT_EQ((std::vector<int> { 1, 2, 3 }), sut.global<std::vector<int>>("x"));

	sut.script("x = { { 1, 2 }, { 3 } }");
	EXPECT_EQ((std::vector<std::vector<int>> { { 1, 2 }, { 3 } }), sut.global<std::vector<std::vector<int>>>("x"));

	sut.script("x = \"test\"");
	EXPECT_STREQ("test", sut.global<char const *>("x"));

	sut.script("x = { 1, \"test\" }");
	EXPECT_EQ((std::make_tuple(1, std::string("test"))), (sut.global<std::tuple<int, std::string>>("x")));

	sut.script("x = 5");
	Value v = sut.global<Value>("x");
	EXPECT_EQ(5, v.get<int>());
}

TEST(VariablesTest, SafelyGetting)
{
	Lua sut;

	sut.script("x = 5");
	std::string str;
	EXPECT_EQ(Error::Code::BadType, std::get<1>(sut.safeGetGlobal("x", str)).code);
	int i;
	Error e;
	std::tie(i, e) = sut.safeGetGlobal("x", i);
	EXPECT_EQ(Error::Code::Ok, e.code);
	EXPECT_EQ(5, i);
}

TEST(VariablesTest, SettinStandardTypes)
{
	Lua sut;

	sut.global("x", true);
	EXPECT_TRUE(sut.global<bool>("x"));

	sut.global("x", 1);
	EXPECT_EQ(1, sut.global<int>("x"));

	sut.global("x", 1.5);
	EXPECT_EQ(1.5, sut.global<float>("x"));

	sut.global("x", std::vector<int> { 1, 2, 3 });
	EXPECT_EQ((std::vector<int> { 1, 2, 3 }) , sut.global<std::vector<int>>("x"));

	sut.global("x", std::vector<std::vector<int>> { { 1, 2 }, { 3 }});
	EXPECT_EQ((std::vector<std::vector<int>> { { 1, 2 }, { 3 } }), sut.global<std::vector<std::vector<int>>>("x"));

	sut.global("x", "test");
	EXPECT_STREQ("test", sut.global<char const *>("x"));

	sut.global("x", std::make_tuple(1, "test"));
	EXPECT_EQ((std::make_tuple(1, std::string("test"))), (sut.global<std::tuple<int, std::string>>("x")));

	{
		int * i = new int(3);
		sut.global("x", i);
		*sut.global<int*>("x") = 5;
		EXPECT_EQ(5, *i);
		delete i;
	}

	{
		struct S
		{
			int a;
		};
		S s = { 3 };
		sut.global("x", s);
		sut.global<S*>("x")->a = 5;
		s = sut.global<S>("x");
		EXPECT_EQ(5, s.a);
	}
}
