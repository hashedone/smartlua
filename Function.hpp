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
#include "impl/Function.hpp"
#include "impl/CallStack.hpp"

#include <string>

namespace smartlua
{

class Function
{
public:
	Function(impl::Reference && ref):
		fnc(std::move(ref))
	{ }

	void push(lua_State * state) const { fnc.getReference().push(state); }

	operator bool() const { return fnc; }

	template<class... Args>
	Error call(Args... args) const
	{
		impl::CallStack callStack(fnc.getReference().getState());
		lua_State * parentThread = callStack.getCurrentState();
		lua_State * thread = lua_newthread(parentThread);
		callStack.setCurrentState(thread);
		AtScopeExit(
			callStack.setCurrentState(parentThread),
			lua_pop(parentThread, 1));

		return fnc(thread, 0, args...);
	}

	template<class R, class... Args>
	std::tuple<typename impl::FunctionTraits<R>::ReturnType, Error> call(Args... args) const
	{
		impl::CallStack callStack(fnc.getReference().getState());
		lua_State * parentThread = callStack.getCurrentState();
		lua_State * thread = lua_newthread(parentThread);
		callStack.setCurrentState(thread);
		AtScopeExit(
			callStack.setCurrentState(parentThread),
			lua_pop(parentThread, 1));

		auto e = fnc(thread, impl::FunctionTraits<R>::returnsCount, args...);
		if(!e)
		{
			return std::make_tuple(typename impl::FunctionTraits<R>::ReturnType(), e);
		}

		return impl::FunctionTraits<R>::extractResult(thread);
	}

	template<class... Args>
	void operator()(Args... args) const
	{
		call(args...);
	}

	template<class R, class...Args>
	typename impl::FunctionTraits<R>::ReturnType operator()(R r, Args... args) const
	{
		return call(r, args...);
	}

private:

	impl::Function fnc;
};

}
