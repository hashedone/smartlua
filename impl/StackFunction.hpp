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
#include "../Function.hpp"

#ifndef SMARTLUA_USER_LUA_INCLUDE
#include <lua.hpp>
#endif

#include <string>
#include <functional>

namespace smartlua { namespace impl
{

template<>
struct StackPusher<smartlua::Function>
{
	static void push(lua_State * state, const smartlua::Function & val)
	{
		val.push(state);
	}
};

template<>
struct StackGetter<smartlua::Function>
{
	static smartlua::Function get(lua_State * state, int idx)
	{
		return smartlua::Function(Reference::createFromStack(state, idx));
	}

	static bool is(lua_State * state, int idx)
	{
		const int top = lua_gettop(state);
		AtScopeExit(lua_settop(state, top));

		if(!lua_isfunction(state, idx))
		{
			lua_getmetatable(state, idx);
			lua_pushstring(state, "__call");
			lua_rawget(state, -1);
			return lua_isfunction(state, -1);
		}

		return true;
	}

	static std::tuple<smartlua::Function, Error> safeGet(lua_State * state, int idx)
	{
		const int top = lua_gettop(state);
		AtScopeExit(lua_settop(state, top));

		if(!lua_isfunction(state, idx))
		{
			lua_getmetatable(state, idx);
			lua_pushstring(state, "__call");
			lua_rawget(state, -1);
			return std::make_tuple(
				std::move(smartlua::Function(Reference(state))),
				Error::badType("function", lua_typename(state, lua_type(state, idx))));
		}

		return std::make_tuple(smartlua::Function(std::move(Reference::createFromStack(state, idx))), Error::noError());
	}
};

template<class... Args>
struct StackGetter<std::function<void(Args...)>>
{
	static std::function<void(Args...)> get(lua_State * state, int idx)
	{
		auto f = Stack::get<smartlua::Function>(state, idx);
		return [f](Args... args) { f.call(args...); };
	}

	static bool is(lua_State * state, int idx)
	{
		return Stack::is<smartlua::Function>(state, idx);
	}

	static std::tuple<std::function<void(Args...)>, Error> safeGet(lua_State * state, int idx)
	{
		auto result = Stack::safeGet<smartlua::Function>(state, idx);
		if(!std::get<Error>(result))
		{
			return std::make_tuple([](Args...){ }, std::get<Error>(result));
		}
		auto f = std::get<smartlua::Function>(result);
		return [f](Args... args) { f.call(args...); };
	}
};

template<class R, class... Args>
struct StackGetter<std::function<void(Args...)>>
{
	static std::function<typename FunctionTraits<R>::ReturnType(Args...)> get(lua_State * state, int idx)
	{
		auto f = Stack::get<smartlua::Function>(state, idx);
		return [f](Args... args) { return std::get<0>(f.call(args...)); };
	}

	static bool is(lua_State * state, int idx)
	{
		return Stack::is<smartlua::Function>(state, idx);
	}

	static std::tuple<std::function<typename FunctionTraits<R>::ReturnType(Args...)>, Error> safeGet(lua_State * state, int idx)
	{
		auto result = Stack::safeGet<smartlua::Function>(state, idx);
		if(!std::get<Error>(result))
		{
			return std::make_tuple([](Args...){ typename FunctionTraits<R>::ReturnType(); }, std::get<Error>(result));
		}
		auto f = std::get<smartlua::Function>(result);
		return [f](Args... args) { return std::get<0>(f.call(args...)); };
	}
};

} }

