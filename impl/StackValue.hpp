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
#include "../Value.hpp"

#ifndef SMARTLUA_USER_LUA_INCLUDE
#include <lua.hpp>
#endif

#include <string>

namespace smartlua { namespace impl
{

template<>
struct StackPusher<Value>
{
	static void push(lua_State * state, const Value & val)
	{
		val.push(state);
	}
};

template<>
struct StackGetter<Value>
{
	static Value get(lua_State * state, int idx)
	{
		return Value(Reference::createFromStack(state, idx));
	}

	static bool is(lua_State *, int)
	{
		return true;
	}

	static std::tuple<Value, Error> safeGet(lua_State * state, int idx)
	{
		return std::make_tuple(Value(Reference::createFromStack(state, idx)), Error::noError());
	}
};

} }

