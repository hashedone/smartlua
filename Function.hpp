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
#include "Error.hpp"
#include "impl/Reference.hpp"

#include <string>

namespace smartlua
{

/**
 * Callable lua function
 */
template<class R>
class Function
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
	R operator()(Args... args)
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
		if(lua_pcall(ref.getState(), sizeof...(Args), 1, 0))
		{
			lastError = Error::runtimeError(
				"function " + name,
				stack.get<std::string>());
			stack.size(pos);
			return R();
		}

		R result;
		lastError = stack.safeGet(result);
		if(!lastError)
			lastError = Error::stackError(
				"function " + name,
				"extracting result",
				lastError.desc);
		else
			lastError = Error::noError();

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

template<>
class Function<void>
{
public:
	Function(impl::Reference && ref_, const std::string & name_ = "__UNKNOWN_FUNCTION__"):
		ref(ref_),
		lastError(Error::noError()),
		name(name_)
	{
		auto state = ref.getState();
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

	Error error() const { return lastError; }
	operator bool() const { return ref && lastError; }

	template<class... Args>
	void operator()(Args... args)
	{
		if(!ref)
		{
			lastError = Error::emptyReferenceUsage(
				"function " + name,
				"function call");
			return;
		}

		ref.push();
		pushArgs(args...);
		if(lua_pcall(ref.getState(), sizeof...(Args), 0, 0))
		{
			Stack stack(ref.getState());
			lastError = Error::runtimeError(
				"function " + name,
				stack.get<std::string>());
			stack.pop(1);
		}
		else
		{
			lastError = Error::noError();
		}
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
