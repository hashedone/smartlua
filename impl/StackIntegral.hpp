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
 */

#pragma once

#include <lua.hpp>

#include <type_traits>
#include <tuple>

namespace smartlua { namespace impl
{

template<class T, class E=void>
struct Stack;

template<class T>
struct Stack<T, typename std::enable_if<std::is_integral<T>::value>::type>
{
	static void push(lua_State * state, T val)
	{
		lua_pushinteger(state, val);
	}

	static T get(lua_State * state, int idx)
	{
		return lua_tointeger(state, idx);
	}

	static bool is(lua_State * state, int idx)
	{
		return lua_isinteger(state, idx);
	}

	static bool safeGet(lua_State * state, T & result, int idx)
	{
		if(lua_isinteger(state, idx))
		{
			result = lua_tointeger(state, idx);
			return true;
		}

		return false;
	}
};

} }
