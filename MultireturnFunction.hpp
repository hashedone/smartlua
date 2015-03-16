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

#include "Function.hpp"

#include <vector>
#include <boost/format.hpp>

namespace smartlua
{

template<class... Rs>
struct MultiReturn { };

/**
 * Function returning any number of arguments with the same type
 */
template<class R>
class Function<MultiReturn<R>>
{
public:
	Function(impl::Reference && ref_, const std::string & name_ = "__UNKNOWN_FUNCTION__"):
		ref(ref_),
		lastError(Error::noError()),
		name(name_)
	{
		auto state = ref_.getState();
		Stack stack(ref.getState());
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
					lastError = Error::badReference(
						"function " + name,
						"function",
						lua_typename(state, lua_type(state, -3)));
				}
			}
		}

		stack.size(pos);
	}

	Error error()
	{
		std::string result = lastError;
		result = Error::noError();
		return result;
	}
	operator bool() const { return ref && lastError; }

	template<class... Args>
	std::vector<R> operator()(Args... args)
	{
		if(!ref)
		{
			lastError = Error::emptyReferenceUsage(
				"function " + name,
				"function call");
			return R();
		}
		Stack stack(ref.getState());
		int pos = stack.size();

		ref.push();
		pushArgs(args...);
		if(lua_pcall(ref.getState(), sizeof...(Args), LUA_MULTRET, 0))
		{
			lastError = Error::runtimeError(
				"function " + name,
				stack.get<std::string>());
			stack.size(pos);
			return R();
		}

		std::vector<R> result;
		auto it = std::inserter(result, result.end());
		for(int i = 1; i <= stack.size(); ++i)
		{
			if(!stack.safeGet(it, i))
			{
				lastError = Error::stackError(
					"function " + name,
					"extracting result",
					// (boost::format("extracting result %1%") % i).str(),
					stack.get<std::string>());
				stack.size(pos);
				return result;
			}
		}

		stack.size(pos);
		return result;
	}

private:
	void pushArgs() { }

	template<class Head, class... Tail>
	void pushArgs(Head head, Tail... tail)
	{
		Stack(ref.getState()).push(head);
		pushArgs(tail...);
	}

	impl::Reference ref;
	Error lastError;
	std::string name;
};

}
