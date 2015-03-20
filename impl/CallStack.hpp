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

#ifndef SMARTLUA_USER_LUA_INCLUDE
#include <lua.hpp>
#endif

#include "../utils/AtScopeExit.hpp"

namespace smartlua { namespace impl
{

class CallStack
{
public:
	explicit CallStack(lua_State * state_):
		state(state_) { }

	void setCurrentState(lua_State * newState)
	{
		lua_pushstring(newState, "smartlua_currentThread");
		lua_pushthread(newState);
		lua_rawset(newState, LUA_REGISTRYINDEX);
	}

	lua_State * getCurrentState() const
	{
		lua_pushstring(state, "smartlua_currentThread");
		lua_rawget(state, LUA_REGISTRYINDEX);
		AtScopeExit(lua_pop(state, 1));
		return lua_tothread(state, -1);
	}

private:
	lua_State * state;
};

} }
