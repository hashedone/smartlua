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

#include "../Error.hpp"

#ifndef SMARTLUA_USER_LUA_INCLUDE
#include <lua.hpp>
#endif

#include <boost/format.hpp>

#include <tuple>

namespace smartlua { namespace impl
{

template<class T, class E=void>
struct StackPusher
{
	static void push(lua_State * state, const T & val)
	{
		static const int metatableRef = prepareMetatable(state); // Same metatable would be used for all instances

		T * ptr = static_cast<T *>(lua_newuserdata(state, sizeof(T)));
		new(ptr) T(val);
		lua_rawgeti(state, LUA_REGISTRYINDEX, metatableRef);
		lua_setmetatable(state, -2);
	}
private:
	static int prepareMetatable(lua_State * state)
	{
		lua_createtable(state, 0, 1);
		lua_pushstring(state, "__gc");
		lua_pushcclosure(state, &StackPusher<T>::gc, 0);
		lua_rawset(state, -3);
		return luaL_ref(state, LUA_REGISTRYINDEX);
	}

	static int gc(lua_State * state)
	{
		T * ptr = static_cast<T*>(lua_touserdata(state, -1));
		ptr->~T();
		return 0;
	}
};

template<class T, class E=void>
struct StackGetter
{
	static T get(lua_State * state, int idx)
	{
		return *static_cast<T *>(lua_touserdata(state, idx));
	}

	static bool is(lua_State * state, int idx)
	{
		return lua_isuserdata(state, idx) || lua_islightuserdata(state, idx);
	}

	static std::tuple<T, Error> safeGet(lua_State * state, int idx)
	{
		if(!(lua_isuserdata(state, idx) || lua_islightuserdata(state, idx)))
			return std::make_tuple(
				T(),
				Error::badType("userdata", lua_typename(state, lua_type(state, idx))));

		return std::make_tuple(
			*static_cast<T*>(lua_touserdata(state, idx)),
			Error::noError());
	}
};

struct Stack
{
	template<class T>
	static void push(lua_State * state, const T & val)
	{
		StackPusher<T>::push(state, val);
	}

	template<class T>
	static T get(lua_State * state, int idx)
	{
		return StackGetter<T>::get(state, idx);
	}

	template<class T>
	static bool is(lua_State * state, int idx)
	{
		return StackGetter<T>::is(state, idx);
	}

	template<class T>
	static std::tuple<T, Error> safeGet(lua_State * state, int idx)
	{
		return StackGetter<T>::safeGet(state, idx);
	}
};

} }
