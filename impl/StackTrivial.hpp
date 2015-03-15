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

#include  <lua.hpp>

#include <type_traits>
#include <cstdlib>

namespace smartlua { namespace impl
{

template<class T>
struct Stack<T, typename std::enable_if<std::is_trivially_destructible<T>::value>::type>
{
	static void push(lua_State * state, const T & val)
	{
		T * ptr = static_cast<T *>(lua_newuserdata(state, sizeof(T)));
		memcpy(ptr, &val, sizeof(T));
		lua_createtable(state, 0, 1);
		lua_pushstring(state, "size");
		lua_pushinteger(state, sizeof(T));
		lua_settable(state, -3);
		lua_setmetatable(state, -2);
	}

	static T get(lua_State * state, int idx)
	{
		return *static_cast<T *>(lua_touserdata(state, idx));
	}

	static bool is(lua_State * state, int idx)
	{
		if(!lua_isuserdata(state, idx))
			return false;

		lua_getmetatable(state, idx);
		lua_pushstring(state, "size");
		lua_gettable(state, -1);
		if(!lua_isinteger(state, -1))
		{
			lua_pop(state, 2);
			return false;
		}
		int size = lua_tointeger(state, -1);
		lua_pop(state, 2);
		return size == sizeof(T);
	}

	template<class U=T>
	static bool safe_get(lua_State * state, U & result, int idx)
	{
		if(!lua_isuserdata(state, idx))
		{
			lua_pushfstring(state, "while getting from stack: expected userdata, %s found",
				lua_typename(state, lua_type(state, idx)));
			return false;
		}

		lua_getmetatable(state, idx);
		lua_pushstring(state, "size");
		lua_gettable(state, -1);
		if(!lua_isinteger(state, -1))
		{
			lua_pushfstring(state, "while getting from stack: size of userdata unknown",
				lua_typename(state, lua_type(state, idx)));
			lua_pop(state, 2);
			return false;
		}
		if(sizeof(T) != lua_tointeger(state, -1))
		{
			lua_pop(state, 2);
			lua_pushfstring(state, "while getting from stack: size of userdata doesn't correspond to requested type",
				lua_typename(state, lua_type(state, idx)));
			return false;
		}
		lua_pop(state, 2);
		result = *static_cast<T>(lua_touserdata(state, idx));
	}
};

} }
