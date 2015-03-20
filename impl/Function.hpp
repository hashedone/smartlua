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

#include "StackReference.hpp"
#include "../Error.hpp"
#include "../utils/AtScopeExit.hpp"
#include "Reference.hpp"

#include <string>
#include <tuple>

namespace smartlua { namespace impl
{

class Function
{
public:
	Function(Reference && ref_, const std::string & name_):
		ref(ref_),
		name(name_)
	{
		auto state = ref_.getState();
		const int top = lua_gettop(state);
		AtScopeExit(lua_settop(state, top));

		if(ref)
		{
			ref.push(state);
			if(!lua_isfunction(state, -1))
			{
				lua_getmetatable(state, -1);
				lua_pushstring(state, "__call");
				lua_rawget(state, -2);
				if(!lua_isfunction(state, -1))
					ref.invalidate();
			}
		}
	}

	operator bool() const { return ref; }
	std::string getName() const { return name; }
	Reference & getReference() { return ref; }
	Reference const & getReference() const { return ref; }

	template<class... Args>
	Error operator()(lua_State * thread, int retc, Args... args) const
	{
		if(!ref)
		{
			return Error::emptyReferenceUsage(
					"function " + name,
					"function call");
		}

		ref.push(thread);

		pushArgs(thread, args...);
		int p = lua_pcall(thread, sizeof...(Args), retc, 0);
		if(p)
		{
			AtScopeExit(lua_settop(thread, 0));
			return Error::runtimeError(
					"function " + name,
					lua_tostring(thread, -1));
		}

		return Error::noError();
	}

private:
	void pushArgs(lua_State *) const { }

	template<class Head, class... Tail>
	void pushArgs(lua_State * thread, Head head, Tail... tail) const
	{
		Stack<Head>::push(thread, head);
		pushArgs(thread, tail...);
	}

	Reference ref;
	std::string name;
};

} }
