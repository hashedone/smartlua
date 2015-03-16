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
struct Stack<bool>
{
	static void push(lua_State * state, bool val)
	{
		lua_pushboolean(state, val);
	}

	static bool get(lua_State * state, int idx)
	{
		return lua_toboolean(state, idx);
	}

	static bool is(lua_State * state, int idx)
	{
		return lua_isboolean(state, idx);
	}

	template<class U>
	static bool safe_get(lua_State * state, U & result, int idx)
	{
		if(!lua_isboolean(state, idx))
			return Error::stackError(
				(boost::format("expected boolean, %1% found")
				% lua_typename(state, lua_type(state, idx))).str());

		result = lua_toboolean(state, idx);
		return Error::noError();
	}
};

} }

