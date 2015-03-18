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

#pragma once

/*
 * This tip is taken from Arthur O'Dwyer lecture
 * on CppCon 2014 "C++11 in the wild: techniques
 * from a real codebase", available on youtube:
 * https://www.youtube.com/watch?v=lKG1m2NkANM
 */

namespace smartlua { namespace utils
{

template<class Lambda>
class AtScopeExitImpl
{
public:
	AtScopeExitImpl(Lambda & action): lambda(action) { }
	~AtScopeExitImpl() { lambda(); }

private:
	Lambda & lambda;
};

} }

#define SMARTLUA_TOKEN_PASTEx(x, y) x ## y
#define SMARTLUA_TOKEN_PASTE(x, y) SMARTLUA_TOKEN_PASTEx(x, y)

#define AtScopeExit_INTERNAL1(lname, aname, ...) \
	auto lname = [&]() { __VA_ARGS__; }; \
	smartlua::utils::AtScopeExitImpl<decltype(lname)> aname(lname);

#define AtScopeExit_INTERNAL2(ctr, ...) \
		AtScopeExit_INTERNAL1( \
			SMARTLUA_TOKEN_PASTE(AtScopeExit_func_, ctr), \
			SMARTLUA_TOKEN_PASTE(AtScopeExit_instance_, ctr), \
			__VA_ARGS__)

#define AtScopeExit(...) AtScopeExit_INTERNAL2(__COUNTER__, __VA_ARGS__)

