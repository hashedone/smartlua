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

#include "Error.hpp"
#include "Stack.hpp"
#include "utils/AtScopeExit.hpp"

#ifndef SMARTLUA_USER_LUA_INCLUDE
#include <lua.hpp>
#endif

#include <memory>
#include <string>
#include <array>
#include <type_traits>

namespace smartlua
{

enum class lib
{
    base,
    package,
    coroutine,
    string,
    os,
    math,
    table,
    debug,
    bit32,
    io,
};

class Lua
{
public:
	Lua():
		state(luaL_newstate(), lua_close)
	{ }

	template<int N>
	explicit Lua(std::array<lib, N> const & libs):
		state(luaL_newstate(), lua_close)
	{
		for(auto & l: libs)
		{
			switch(l)
			{
            case lib::base:
                luaL_requiref(state.get(), "base", luaopen_base, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::package:
                luaL_requiref(state.get(), "package", luaopen_package, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::coroutine:
                luaL_requiref(state.get(), "coroutine", luaopen_coroutine, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::string:
                luaL_requiref(state.get(), "string", luaopen_string, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::table:
                luaL_requiref(state.get(), "table", luaopen_table, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::math:
                luaL_requiref(state.get(), "math", luaopen_math, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::bit32:
                luaL_requiref(state.get(), "bit32", luaopen_bit32, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::io:
                luaL_requiref(state.get(), "io", luaopen_io, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::os:
                luaL_requiref(state.get(), "os", luaopen_os, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::debug:
                luaL_requiref(state.get(), "debug", luaopen_debug, 1);
                lua_pop(state.get(), 1);
                break;
			}
		}
	}

	explicit Lua(std::initializer_list<lib> const & libs):
		state(luaL_newstate(), lua_close)
	{
		for(auto & l: libs)
		{
			switch(l)
			{
            case lib::base:
                luaL_requiref(state.get(), "base", luaopen_base, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::package:
                luaL_requiref(state.get(), "package", luaopen_package, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::coroutine:
                luaL_requiref(state.get(), "coroutine", luaopen_coroutine, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::string:
                luaL_requiref(state.get(), "string", luaopen_string, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::table:
                luaL_requiref(state.get(), "table", luaopen_table, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::math:
                luaL_requiref(state.get(), "math", luaopen_math, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::bit32:
                luaL_requiref(state.get(), "bit32", luaopen_bit32, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::io:
                luaL_requiref(state.get(), "io", luaopen_io, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::os:
                luaL_requiref(state.get(), "os", luaopen_os, 1);
                lua_pop(state.get(), 1);
                break;
            case lib::debug:
                luaL_requiref(state.get(), "debug", luaopen_debug, 1);
                lua_pop(state.get(), 1);
                break;
			}
		}
	}

	Error script(std::string const & str)
	{
		AtScopeExit(lua_settop(state.get(), 0));
		if(luaL_dostring(state.get(), str.c_str()))
		{
			return Error::runtimeError(lua_tostring(state.get(), -1));
		}
		return Error::noError();
	}

	Error script(char const * str)
	{
		AtScopeExit(lua_settop(state.get(), 0));
		if(luaL_dostring(state.get(), str))
		{
			return Error::runtimeError(lua_tostring(state.get(), -1));
		}
		return Error::noError();
	}

	Error file(std::string const & fname)
	{
		AtScopeExit(lua_settop(state.get(), 0));
		if(luaL_dofile(state.get(), fname.c_str()))
		{
			return Error::runtimeError(lua_tostring(state.get(), -1));
		}
		return Error::noError();
	}

	Error file(char const * fname)
	{
		AtScopeExit(lua_settop(state.get(), 0));
		if(luaL_dofile(state.get(), fname))
		{
			return Error::runtimeError(lua_tostring(state.get(), -1));
		}
		return Error::noError();
	}

	template<class T>
	T global(std::string const & name)
	{
		AtScopeExit(lua_settop(state.get(), 0));
		lua_getglobal(state.get(), name.c_str());
		return Stack(state.get()).get<T>();
	}

	template<class T>
	T global(char const * name)
	{
		AtScopeExit(lua_settop(state.get(), 0));
		lua_getglobal(state.get(), name);
		return Stack(state.get()).get<T>();
	}

	template<class T>
	T isGlobalTypeOf(std::string const & name)
	{
		AtScopeExit(lua_settop(state.get(), 0));
		lua_getglobal(state.get(), name.c_str());
		Stack(state.get()).is<T>();
	}

	template<class T>
	T isGlobalTypeOf(char const * name)
	{
		AtScopeExit(lua_settop(state.get(), 0));
		lua_getglobal(state.get(), name);
		Stack(state.get()).is<T>();
	}

	template<class T>
	Error safeGetGlobal(std::string const & name, T & result)
	{
		AtScopeExit(lua_settop(state.get(), 0));
		lua_getglobal(state.get(), name.c_str());
		return Stack(state.get()).safeGet(result);
	}

	template<class T>
	Error safeGetGlobal(char const * name, T & result)
	{
		AtScopeExit(lua_settop(state.get(), 0));
		lua_getglobal(state.get(), name);
		return Stack(state.get()).safeGet(result);
	}

	template<class T>
	void global(std::string const & name, const T & val)
	{
		Stack(state.get()).push(val);
		lua_setglobal(state.get(), name.c_str());
	}

	template<class T>
	void global(char const * name, const T & val)
	{
		Stack(state.get()).push(val);
		lua_setglobal(state.get(), name);
	}

private:
	std::unique_ptr<lua_State, void(*)(lua_State *)> state;
};

}
