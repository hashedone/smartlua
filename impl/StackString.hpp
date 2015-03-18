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
		return lua_isstring(state, idx) && !lua_isnumber(state, idx);
	}

	static std::tuple<std::string, Error> safeGet(lua_State * state, int idx)
	{
		if(!lua_isstring(state, idx) || lua_isnumber(state, idx))
		{
			return std::make_tuple(
				"",
				Error::badType("string", lua_typename(state, lua_type(state, idx))));
		}

		return std::make_tuple(
				lua_tostring(state, idx),
			Error::noError());
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
		return lua_isstring(state, idx) && !lua_isnumber(state, idx);
	}

	static std::tuple<const char *, Error> safeGet(lua_State * state, int idx)
	{
		if(!lua_isstring(state, idx) || lua_isnumber(state, idx))
			return std::make_tuple(
				"",
				Error::badType("cstring", lua_typename(state, lua_type(state, idx))));

		return std::make_tuple(
			lua_tostring(state, idx),
			Error::noError());
	}
};

} }

