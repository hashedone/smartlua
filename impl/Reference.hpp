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

#include <string>

namespace smartlua { namespace impl
{

/**
 * Class implementing reference counting in lua to keep alive lua object
 * when in use by C++ code
 */
class Reference
{
public:
	Reference() = delete;

	explicit Reference(lua_State * state_):
		Reference(state_, LUA_NOREF)
	{ }

	Reference(lua_State * state_, int ref):
		state(state_),
		ref(ref)
	{ }

	Reference(const Reference & other):
		state(other.state)
	{
		lua_rawgeti(state, LUA_REGISTRYINDEX, other.ref);
		ref = luaL_ref(state, LUA_REGISTRYINDEX);
	}

	Reference(Reference && other):
		Reference(other.state)
	{
		std::swap(ref, other.ref);
	}

	~Reference()
	{
		luaL_unref(state, LUA_REGISTRYINDEX, ref);
	}

	Reference & operator =(const Reference & other)
	{
		luaL_unref(state, LUA_REGISTRYINDEX, ref);
		*this = other.clone();
		return *this;
	}

	Reference & operator=(Reference && other)
	{
		std::swap(state, other.state);
		std::swap(ref, other.ref);
		return *this;
	}

	lua_State * getState() const { return state; }

	void push() const
	{
		lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
	}

	void push(lua_State * state_) const
	{
		lua_rawgeti(state_, LUA_REGISTRYINDEX, ref);
	}

	operator bool() const
	{
		return ref != LUA_NOREF;
	}

	void invalidate()
	{
		luaL_unref(state, LUA_REGISTRYINDEX, ref);
		ref = LUA_NOREF;
	}

	Reference clone() const
	{
		push();
		return Reference(state, luaL_ref(state, LUA_REGISTRYINDEX));
	}

	static Reference createFromStack(lua_State * state, int idx = -1)
	{
		lua_pushvalue(state, idx);
		auto result = Reference(state, luaL_ref(state, LUA_REGISTRYINDEX));
		lua_pop(state, 1);
		return result;
	}

	static Reference createFromGlobal(lua_State * state, const std::string & name)
	{
		lua_getglobal(state, name.c_str());
		auto result = createFromStack(state);
		lua_pop(state, 1);
		return result;
	}

private:
	lua_State * state;
	int ref;
};

} }
