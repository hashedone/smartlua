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
#include "../utils/Traits.hpp"
#include "../utils/AtScopeExit.hpp"
#include "../Error.hpp"

#ifndef SMARTLUA_USER_LUA_INCLUDE
#include <lua.hpp>
#endif

#include <type_traits>
#include <iterator>

namespace smartlua { namespace impl
{

template<class T>
struct StackPusher<T, typename std::enable_if<
	!std::is_void<typename T::value_type>::value &&
	!std::is_void<decltype(std::declval<T&>().begin())>::value &&
	!std::is_void<decltype(std::declval<T&>().end())>::value
	>::type>
{
	static void push(lua_State * state, const T & val)
	{
		lua_newtable(state);
		int i = 1;
		for(auto & item: val)
		{
			lua_pushinteger(state, i++);
			Stack::push(state, item);
			lua_settable(state, -3);
		}
	}
};

template<class T>
struct StackGetter<T, typename std::enable_if<
	!std::is_void<typename T::value_type>::value &&
	!std::is_void<decltype(std::inserter(std::declval<T&>(), std::declval<T&>().end()))>::value
	>::type>
{
	static T get(lua_State * state, int idx)
	{
		const int aidx = lua_absindex(state, idx);
		AtScopeExit(lua_settop(state, aidx));

		T result;
		auto it = std::inserter(result, result.end());
		lua_pushinteger(state, 1);
		lua_gettable(state, aidx);
		for(int i=2; !lua_isnil(state, -1); ++i)
		{
			it = Stack::get<typename T::value_type>(state, -1);
			lua_settop(state, aidx);
			lua_pushinteger(state, i);
			lua_gettable(state, aidx);
		}

		return result;
	}

	static bool is(lua_State * state, int idx)
	{
		const int top = lua_gettop(state);
		const int aidx = lua_absindex(state, idx);
		AtScopeExit(lua_settop(state, top));

		if(!lua_istable(state, aidx))
		{
			lua_getmetatable(state, aidx);
			lua_pushstring(state, "__index");
			lua_rawget(state, -2);
			if(!lua_isfunction(state, -1))
				return false;
		}

		for(int i=1; !lua_isnil(state, -1); ++i){
			lua_settop(state, top);
			lua_pushinteger(state, i);
			if(!Stack::is<typename T::value_type>(state, -1))
				return false;
		}

		return true;
	}

	static std::tuple<T, Error> safeGet(lua_State * state, int idx)
	{
		const int top = lua_gettop(state);
		const int aidx = lua_absindex(state, idx);
		AtScopeExit(lua_settop(state, top));

		if(!lua_istable(state, aidx))
		{
			lua_getmetatable(state, aidx);
			lua_pushstring(state, "__index");
			lua_rawget(state, -2);
			if(!lua_isfunction(state, -1))
				return std::make_tuple(
					T(),
					Error::badType("iterable", lua_typename(state, lua_type(state, aidx))));
		}

		T result;
		Error e;

		auto it = std::inserter(result, result.end());
		for(int i=1; !lua_isnil(state, -1); ++i)
		{
			lua_settop(state, top);
			lua_pushinteger(state, i);
			lua_gettable(state, idx);

			std::tie(it, e) = Stack::safeGet<typename T::value_type>(state, it, -1);
			if(!e)
			{
				e.desc = boost::format("iterable[%1%]: %2%") % (i - 1) % e.desc;
				return std::make_tuple(result, e);
			}
		}

		return std::make_tuple(result, Error::noError());
	}
};

} }
