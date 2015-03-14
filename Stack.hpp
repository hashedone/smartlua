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
 */

#pragma once

#include <lua.hpp>
#include "impl/StackIntegral.hpp"

namespace smartlua
{

class Stack
{
public:
	Stack(lua_State * state_):
		state(state_)
	{ }

	void pushNil() { lua_pushnil(state); }

	int size() const { return lua_gettop(state); }
	void size(int newsize) { lua_settop(state, newsize); }
	void pop(int count) { lua_pop(state, count); }

	template<class T>
	void push(T const & t) { impl::Stack<T>::push(state, t); }

	template<class T>
	T get(int idx = -1) { return impl::Stack<T>::get(state, idx); }

	template<class T>
	bool is(int idx = -1) { return impl::Stack<T>::is(state, idx); }

	template<class T>
	bool safeGet(T & result, int idx = -1) { return impl::Stack<T>::safeGet(state, result, idx); }

private:
	lua_State * state;
};

}
