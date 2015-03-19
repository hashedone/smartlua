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

#include "../Stack.hpp"
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
		smartlua::Stack stack(ref.getState());
		const int top = stack.size();
		AtScopeExit(stack.size(top));

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

	operator bool() const const { return ref; }
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

		smartlua::Stack stack(thread);
		stack.push(ref);

		pushArgs(thread, args...);
		if(lua_pcall(ref.getState(), sizeof...(Args), retc, 0))
		{
			AtScopeExit(stack.size(0));
			return Error::runtimeError(
					"function " + name,
					stack.get<std::string>());
		}

		return Error::noError();
	}

private:
	void pushArgs(lua_State *) const { }

	template<class Head, class... Tail>
	void pushArgs(lua_State * thread, Head head, Tail... tail) const
	{
		smartlua::Stack(thread).push(head);
		pushArgs(tail...);
	}

	Reference ref;
	std::string name;
};

} }
