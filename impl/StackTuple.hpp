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
#include "../utils/Traits.hpp"
#include "../utils/AtScopeExit.hpp"
#include "../Error.hpp"

#ifndef SMARTLUA_USER_LUA_INCLUDE
#include <lua.hpp>
#endif

#include <tuple>
#include <array>

namespace smartlua { namespace impl
{

template<int N, class Tuple>
struct StackTupleHelper
{
	static constexpr int tableTypes = StackTupleHelper<N-1, Tuple>::tableTypes +
		(utils::is_luatable_type<typename std::tuple_element<N-1, Tuple>::type>::value ? 1 : 0);

	static void push(lua_State * state, const Tuple & t)
	{
		lua_pushinteger(state, N);
		Stack<typename std::tuple_element<N-1, Tuple>::type>::push(state, std::get<N-1>(t));
		lua_settable(state, -3);

		StackTupleHelper<N-1, Tuple>::push(state, t);
	}

	static void get(lua_State * state,  Tuple & t, int idx)
	{
		lua_pushinteger(state, N);
		lua_gettable(state, idx);
		std::get<N-1>(t) = Stack<typename std::tuple_element<N-1, Tuple>::type>::get(state, -1);
		lua_pop(state, 1);

		StackTupleHelper<N-1, Tuple>::get(state, t, idx);
	}

	static bool is(lua_State * state, int idx)
	{
		lua_pushinteger(state, N);
		lua_gettable(state, idx);
		if(!Stack<typename std::tuple_element<N-1, Tuple>::type>::is(state, -1))
		{
			lua_pop(state, 1);
			return false;
		}
		lua_pop(state, 1);

		return StackTupleHelper<N-1, Tuple>::is(state, idx);
	}

	static Error safeGet(lua_State * state, Tuple & t, int idx)
	{
		lua_pushinteger(state, N);
		lua_gettable(state, idx);
		Error e;
		std::tie(std::get<N-1>(t), e) = Stack<typename std::tuple_element<N-1, Tuple>::type>::safe_get(state, idx);
		if(!e)
		{
			e.desc = boost::format("tuple[%1%]: %2%") % N % e.desc;
			return e;
		}
		lua_pop(state, 1);

		return StackTupleHelper<N-1, Tuple>::safe_get(state, t, idx);
	}
};

template<class Tuple>
struct StackTupleHelper<0, Tuple>
{
	static constexpr int tableTypes = 0;

	static void push(lua_State *, const Tuple &) { }
	static void get(lua_State *, Tuple &, int) { }
	static bool is(lua_State *, int) { return true; }
};

template<class... Args>
struct Stack<std::tuple<Args...>>
{
	static void push(lua_State * state, const std::tuple<Args...> & t)
	{
		lua_createtable(state,
			StackTupleHelper<sizeof...(Args), std::tuple<Args...>>::tableTypes,
			sizeof...(Args) - StackTupleHelper<sizeof...(Args), std::tuple<Args...>>::tableTypes);
		StackTupleHelper<sizeof...(Args), std::tuple<Args...>>::push(state, t);
	}

	template<class U=std::tuple<Args...>>
	static U get(lua_State * state, int idx)
	{
		std::tuple<Args...> result;
		StackTupleHelper<sizeof...(Args), std::tuple<Args...>>::get(state, result, lua_absindex(state, idx));
		return result;
	}

	static bool is(lua_State * state, int idx)
	{
		return StackTupleHelper<sizeof...(Args), std::tuple<Args...>>::is(state, lua_absindex(state, idx));
	}

	static std::tuple<std::tuple<Args...>, Error> safeGet(lua_State * state, int idx)
	{
		const int top = lua_gettop(state);
		const int aidx = lua_absindex(state, idx);
		AtScopeExit(lua_settop(state, top));

		if(!lua_istable(state, aidx))
		{
			lua_getmetatable(state, aidx);
			lua_getfield(state, -1, "__index");
			if(!lua_isfunction(state, -1))
				return std::make_tuple(
					std::tuple<Args...>(),
					Error::badType("tuple", lua_typename(state, lua_type(state, idx))));
		}

		std::tuple<Args...> result;
		auto e = StackTupleHelper<sizeof...(Args), std::tuple<Args...>>::safeGet(state, result, lua_absindex(state, idx));
		return std::make_tuple(result, e);
	}
};

template<class T, int N>
struct Stack<std::array<T, N>>
{
	static void push(lua_State * state, const std::array<T, N> & t)
	{
		lua_createtable(state,
			StackTupleHelper<N, std::array<T, N>>::tableTypes,
			N - StackTupleHelper<N, std::array<T, N>>::tableTypes);
		StackTupleHelper<N, std::array<T, N>>::push(state, t);
	}

	template<class U=std::array<T, N>>
	static U get(lua_State * state, int idx)
	{
		std::array<T, N> result;
		StackTupleHelper<N, std::array<T, N>>::get(state, result, lua_absindex(state, idx));
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
		return StackTupleHelper<N, std::array<T, N>>::is(state, lua_absindex(state, idx));
	}

	static std::tuple<std::array<T, N>, Error> safeGet(lua_State * state, int idx)
	{
		const int top = lua_gettop(state);
		const int aidx = lua_absindex(state, idx);
		AtScopeExit(lua_settop(state, top));

		if(!lua_istable(state, aidx))
		{
			lua_getmetatable(state, aidx);
			lua_getfield(state, -1, "__index");
			if(!lua_isfunction(state, -1))
				return std::make_tuple(
					std::array<T, N>(),
					Error::badType("array", lua_typename(state, lua_type(state, idx))));
		}

		std::array<T, N> result;
		auto e = StackTupleHelper<N, std::array<T, N>>::safeGet(state, result, lua_absindex(state, idx));
		return std::make_tuple(result, e);
	}
};

} }
