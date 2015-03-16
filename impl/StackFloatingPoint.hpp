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

#include "Stack.hpp"
#include "../Error.hpp"

#include <lua.hpp>

#include <type_traits>

#pragma once

namespace smartlua { namespace impl
{

template<class T>
struct Stack<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
{
	static void push(lua_State * state, T val)
	{
		lua_pushnumber(state, val);
	}

	static T get(lua_State * state, int idx)
	{
		return lua_tonumber(state, idx);
	}

	static bool is(lua_State * state, int idx)
	{
		return lua_isnumber(state, idx);
	}

	template<typename U=T>
	static Error safeGet(lua_State * state, U & result, int idx)
	{
		if(!lua_isnumber(state, idx))
			return Error::stackError(
				(boost::format("expected floating point, %1% found")
				% lua_typename(state, lua_type(state, idx))).str());

		result = lua_tonumber(state, idx);
		return Error::noError();
	}
};

} }
