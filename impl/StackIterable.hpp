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

#include <lua.hpp>

#include <type_traits>
#include <iterator>

namespace smartlua { namespace impl
{

template<class T, class E=void>
struct is_iterable: std::false_type { };

template<class T>
struct is_iterable<T,
	typename std::enable_if<std::is_pointer<decltype(&T::begin)>::value>::type>: std::true_type { };

template<class T>
struct Stack<T, typename std::enable_if<
	!std::is_void<typename T::value_type>::value &&
	!std::is_void<decltype(std::declval<T&>().begin())>::value &&
	!std::is_void<decltype(std::declval<T&>().end())>::value
	>::type>
{
	template<class U=T, void(
		decltype(std::declval<U&>().size()),
		typename std::enable_if<utils::is_luatable_type<U>::value>::type)>
	static void push(lua_State * state, const U & val)
	{
		lua_createtable(state, val.size(), 0);
		int i = 1;
		for(auto & item: val)
		{
			lua_pushinteger(state, i++);
			Stack<typename U::value_type>::push(state, item);
			lua_settable(state, -3);
		}
	}

	template<class U=T, void(
		decltype(std::declval<U&>().size()),
		typename std::enable_if<!utils::is_luatable_type<U>::value>::type)>
	static void push(lua_State * state, const U & val)
	{
		lua_createtable(state, 0, val.size());
		int i = 1;
		for(auto & item: val)
		{
			lua_pushinteger(state, i++);
			Stack<typename U::value_type>::push(state, item);
			lua_settable(state, -3);
		}
	}

	template<class U=T>
	static void push(lua_State * state, const U & val)
	{
		lua_newtable(state);
		int i = 1;
		for(auto & item: val)
		{
			lua_pushinteger(state, i++);
			Stack<typename U::value_type>::push(state, item);
			lua_settable(state, -3);
		}
	}

	template<class U=T, class E=
		decltype(std::inserter(std::declval<U&>(), std::declval<U&>().end()))>
	static U get(lua_State * state, int idx)
	{
		idx = lua_absindex(state, idx);
		U result;
		auto it = std::inserter(result, result.end());
		for(int i=1;; ++i)
		{
			lua_pushinteger(state, i);
			lua_gettable(state, idx);
			if(lua_isnil(state, -1))
			{
				lua_pop(state, 1);
				return result;
			}
			it = Stack<typename U::value_type>::get(state, -1);
			lua_pop(state, 1);
		}
	}

	static bool is(lua_State * state, int idx)
	{
		return lua_istable(state, idx);
	}

	template<class U=T, class E=
			decltype(std::inserter(std::declval<T&>(), std::declval<T&>().end()))>
	static bool safe_get(lua_State * state, U & result, int idx)
	{
		if(!lua_istable(state, idx))
		{
			lua_pushfstring(state, "while getting from stack: expected iterable, %s found",
				lua_typename(state, lua_type(state, idx)));

			return false;
		}
		idx = lua_absindex(state, idx);
		T tmpResult;
		auto it = std::inserter(tmpResult, tmpResult.end());
		for(int i=1;; ++i)
		{
			lua_pushinteger(state, i);
			lua_gettable(state, idx);
			if(lua_isnil(state, -1))
			{
				lua_pop(state, 1);
				result = std::move(tmpResult);
				return true;
			}
			if(!Stack<typename T::value_type>::safe_get(state, it, -1))
			{
				lua_pushfstring(state, "while getting iterable[%d] from stack: %s",
					i,
					lua_tostring(state, -1));
				lua_replace(state, -3);
				lua_pop(state, 1);
				return false;
			}
			lua_pop(state, 1);
		}
	}
};

} }
