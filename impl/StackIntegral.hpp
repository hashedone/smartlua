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

#ifndef SMARTLUA_USER_LUA_INCLUDE
#include <lua.hpp>
#endif

#include <type_traits>

namespace smartlua { namespace impl
{

template<class T>
struct StackPusher<T, typename std::enable_if<std::is_integral<T>::value>::type>
{
	static void push(lua_State * state, T val)
	{
		lua_pushinteger(state, val);
	}
};

template<class T>
struct StackGetter<T, typename std::enable_if<std::is_integral<T>::value>::type>
{
	static T get(lua_State * state, int idx)
	{
		return lua_tointeger(state, idx);
	}

	static bool is(lua_State * state, int idx)
	{
		return lua_isinteger(state, idx);
	}

	static std::tuple<T, Error> safeGet(lua_State * state, int idx)
	{
		if(!lua_isinteger(state, idx))
			return std::make_tuple(
				0,
				Error::badType("integral", lua_typename(state, lua_type(state, idx))));

		return std::make_tuple(
			lua_tointeger(state, idx),
			Error::noError());
	}
};

} }
