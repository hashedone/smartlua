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

#include "impl/StackIntegral.hpp"
#include "impl/StackFloatingPoint.hpp"
#include "impl/StackIterable.hpp"
#include "impl/StackTuple.hpp"

#include <lua.hpp>

namespace smartlua
{

/**
 * Helper class for stack manipulations
 */
class Stack
{
public:
	Stack(lua_State * state_):
		state(state_)
	{ }

	/**
	 * Rushes nil to lua stack
	 */
	void pushNil() { lua_pushnil(state); }

	/**
	 * \return Number of elements on stack
	 */
	int size() const { return lua_gettop(state); }
	/**
	 * Resizes lua stack
	 * \param newsize New size of lua stack
	 */
	void size(int newsize) { lua_settop(state, newsize); }
	/**
	 * Removes elements from top of lua stack
	 * \param count Number of elements to be removed
	 */
	void pop(int count) { lua_pop(state, count); }

	/**
	 * Pushes an element to lua stack
	 * \param t Element to be pushed
	 */
	template<class T>
	void push(T const & t) { impl::Stack<T>::push(state, t); }

	/**
	 * Gets an element from lua stack
	 *
	 * Elements are indexed from the bottom of stack starting from 1, but can be indexed with
	 * negative values, what means indexing from the top of stack. By default the element
	 * from the top of stack will be returned.
	 * Returned element is automatically converted by lua to requested type, if the conversion
	 * would fail behavior is defined by lua library - while documenting this code it is
	 * long_jump to to lua error handler which ends with application terminate - can be
	 * changed by lua_atpanic function. It is not recommended to use this function if
	 * the type on the top of stack is not known for sure
	 *
	 * \param idx Index of element to be returned
	 * \return Indexed element from lua stack
	 */
	template<class T>
	T get(int idx = -1) { return impl::Stack<T>::get(state, idx); }

	/**
	 * Checks is an element on lua stack is comatible with given type
	 *
	 * Elements are indexed from the botom of stack starting from 1, but can be indexed with
	 * negative values, what means indexing from the top of stack. By default the element
	 * from the top of stack will be returned.
	 *
	 * \param T Type to check if element on the stack is compatible with
	 * \param idx Index of element to be checked if is compatible with given type
	 * \return True if element of lua stack is compatible with given type, false otherwise
	 */
	template<class T>
	bool is(int idx = -1) { return impl::Stack<T>::is(state, idx); }

	/**
	 * Gets an element from lua stack if is compatible type
	 *
	 * Elements are indexed from the bottom of stack starting from 1, but can be indexed with
	 * negative values, what means indexing from the top of stack. By default the element
	 * from the top of stack will be returned.
	 *
	 * \param result[out] Variable to put extracted result
	 * \param idx Index of element to be returned
	 * \return True if element of lua stack is compatible with given type, false otherwise
	 */
	template<class T>
	bool safeGet(T & result, int idx = -1) { return impl::Stack<T>::safeGet(state, result, idx); }

private:
	lua_State * state;
};

}
