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

#include "Stack.hpp"
#include "../Error.hpp"

#include <lua.hpp>

#include <string>

namespace smartlua { namespace impl
{

template<>
struct Stack<std::string>
{
	static void push(lua_State * state, const std::string & str)
	{
		lua_pushstring(state, str.c_str());
	}

	static std::string get(lua_State * state, int idx)
	{
		return lua_tostring(state, idx);
	}

	static bool is(lua_State * state, int idx)
	{
		return lua_isstring(state, idx);
	}

	template<class U>
	static Error safe_get(lua_State * state, U & str, int idx)
	{
		if(!lua_isstring(state, idx))
			return Error::stackError(
				(boost::format("expected string, %1% found")
				% lua_typename(state, lua_type(state, idx))).str());

		str = lua_tostring(state, idx);
		return Error::noError();
	}
};

template<>
struct Stack<const char *>
{
	static void push(lua_State * state, const char * str)
	{
		lua_pushstring(state, str);
	}

	static const char * get(lua_State * state, int idx)
	{
		return lua_tostring(state, idx);
	}

	static bool is(lua_State * state, int idx)
	{
		return lua_isstring(state, idx);
	}

	template<class U>
	static Error safe_get(lua_State * state, U & str, int idx)
	{
		if(!lua_isstring(state, idx))
			return Error::stackError(
				(boost::format("expected cstring, %1% found")
				% lua_typename(state, lua_type(state, idx))).str());

		str = lua_tostring(state, idx);
		return Error::noError();
	}
};

} }

