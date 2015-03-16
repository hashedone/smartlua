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
#include "Reference.hpp"

#include <string>
#include <tuple>

namespace smartlua { namespace impl
{

class Function
{
public:
	Function(impl::Reference && ref_, const std::string & name_, Error & error):
		ref(ref_),
		name(name_)
	{
		error = Error::noError();

		auto state = ref_.getState();
		smartlua::Stack stack(ref.getState());
		int pos = stack.size();

		if(ref)
		{
			ref.push();
			if(!lua_isfunction(state, -1))
			{
				lua_getmetatable(state, -1);
				lua_pushstring(state, "__call");
				lua_rawget(state, -2);
				if(!lua_isfunction(state, -1))
				{
					ref.invalidate();
					error = Error::badReference(
						"function " + name,
						"function",
						lua_typename(state, lua_type(state, -3)));
				}
			}
		}

		stack.size(pos);
	}

	operator bool() const { return ref; }
	const std::string getName() { return name; }

	template<class... Args>
	std::tuple<lua_State *, Error> operator()(int retc, Args... args)
	{
		if(!ref)
		{
			return
			std::make_tuple(
				ref.getState(),
				Error::emptyReferenceUsage(
					"function " + name,
					"function call")
			);
		}

		smartlua::Stack stack(ref.getState());
		ref.push();

		pushArgs(args...);
		if(lua_pcall(ref.getState(), sizeof...(Args), retc, 0))
		{
			auto e = Error::runtimeError(
				"function " + name,
				stack.get<std::string>());
			stack.pop();
			return std::make_tuple(ref.getState(), e);
		}

		return std::make_tuple(ref.getState(), Error::noError());
	}

private:
	void pushArgs() { }

	template<class Head, class... Tail>
	void pushArgs(Head head, Tail... tail)
	{
		smartlua::Stack(ref.getState()).push(head);
		pushArgs(tail...);
	}

	impl::Reference ref;
	std::string name;
};

} }
