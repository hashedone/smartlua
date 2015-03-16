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
struct Stack<T, typename std::enable_if<std::is_trivially_destructible<T>::value && !std::is_fundamental<T>::value>::type>
{
	static void push(lua_State * state, const T & val)
	{
		T * ptr = static_cast<T *>(lua_newuserdata(state, sizeof(T)));
		memcpy(ptr, &val, sizeof(T));
	}

	static T get(lua_State * state, int idx)
	{
		return *static_cast<T *>(lua_touserdata(state, idx));
	}

	static bool is(lua_State * state, int idx)
	{
		return lua_isuserdata(state, idx) || lua_islightuserdata(state, idx);
	}

	template<class U=T>
	static bool safe_get(lua_State * state, U & result, int idx)
	{
		if(!(lua_isuserdata(state, idx) || lua_islightuserdata(state, idx)))
		{
			lua_pushfstring(state, "while getting from stack: expected userdata, %s found",
				lua_typename(state, lua_type(state, idx)));
			return false;
		}

		result = *static_cast<T>(lua_touserdata(state, idx));
		return true;
	}
};

} }
