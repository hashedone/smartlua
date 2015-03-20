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
#include "../utils/Traits.hpp"

#ifndef SMARTLUA_USER_LUA_INCLUDE
#include <lua.hpp>
#endif

#include <type_traits>
#include <cstdlib>

namespace smartlua { namespace impl
{

template<class T>
struct Stack<T,
	typename std::enable_if<
		std::is_trivially_destructible<T>::value &&
		!std::is_fundamental<T>::value &&
		!std::is_pointer<T>::value &&
		!std::is_array<T>::value &&
		!utils::is_luatable_type<T>::value>::type>
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

	static std::tuple<T, Error> safeGet(lua_State * state, int idx)
	{
		if(!(lua_isuserdata(state, idx) || lua_islightuserdata(state, idx)))
			return std::make_pair(
				T(),
				Error::badType("pod", lua_typename(state, lua_type(state, idx))));

		return std::make_tuple(
			*static_cast<T>(lua_touserdata(state, idx)),
			Error::noError());
	}
};

} }
