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
#include "../Function.hpp"

#include <gtest/gtest.h>

using namespace smartlua;

TEST(FunctionShould, Call)
{
	Lua lua;
	lua.script("function foo(a, b) g = a + b end");
	auto foo = lua.global<Function>("foo");
	ASSERT_TRUE(foo);
	ASSERT_EQ(Error::noError().code, foo.call(4, 5).code);
	ASSERT_EQ(9, lua.global<int>("g"));
}
