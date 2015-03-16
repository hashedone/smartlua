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

template<class T>
struct Stack<T*>
{
	static void push(lua_State * state, T * val)
	{
		lua_pushlightuserdata(state, static_cast<void*>(val));
	}

	static bool get(lua_State * state, int idx)
	{
		return static_cast<T*>(lua_touserdata(state, idx));
	}

	static bool is(lua_State * state, int idx)
	{
		return lua_islightuserdata(state, idx) || lua_isuserdata(state, idx);
	}

	template<class U>
	static Error safe_get(lua_State * state, U & result, int idx)
	{
		if(!(lua_islightuserdata(state, idx) || lua_isuserdata(state, idx)))
			return Error::stackError(
				(boost::format("expected pointer, %1% found")
				% lua_typename(state, lua_type(state, idx))).str());

		result = static_cast<T*>(lua_touserdata(state, idx));
		return Error::noError();
	}
};

} }

