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
#include "impl/Function.hpp"
#include "impl/CallStack.hpp"

#include <string>

namespace smartlua
{

class Function
{
public:
	Function(impl::Reference && ref, const std::string & name = "__UNKNOWN_FUNCTION__"):
		fnc(std::move(ref), name)
	{ }

	void push(lua_State * state) const { fnc.getReference().push(state); }

	operator bool() const { return fnc; }

	template<class... Args>
	Error call(Args... args)
	{
		// Creating new stack frame
		impl::CallStack callStack(fnc.getReference().getState());
		lua_State * const parentThread = callStack.getCurrentState();
		lua_State * thread = lua_newthread(parentThread);
		callStack.pushState(thread);
		AtScopeExit(
			callStack.popState(),
			lua_pop(parentThread, 1));

		return fnc(thread, 0, args...);
	}

	/*template<class R, class... Args>
	R operator()(Args... args)
	{
		lua_State * state;
		std::tie(state, lastError) = fnc(1, args...);
		Stack stack(state);
		if(!lastError)
		{
			stack.size(0);
			return R();
		}

		R result;
		lastError = stack.safeGet(result);
		if(!lastError)
			lastError = Error::stackError(
				"function " + fnc.getName(),
				"extracting result",
				lastError.desc);
		else
			lastError = Error::noError();

		stack.size(0);
		return result;
	}*/

private:

	impl::Function fnc;
};

/*template<>
class Function<void>
{
public:
	Function(impl::Reference && ref, const std::string & name = "__UNKNOWN_FUNCTION__"):
		lastError(Error::noError()),
		fnc(std::move(ref), name, lastError)
	{ }

	Error error() const { return lastError; }
	operator bool() const { return fnc; }

	template<class... Args>
	void operator()(Args... args)
	{
		lua_State * state;
		std::tie(state, lastError) = fnc(0, args...);
		Stack(state).size(0);
	}

private:
	Error lastError;
	impl::Function fnc;
};*/

}
