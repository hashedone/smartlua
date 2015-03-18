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

#include "impl/Reference.hpp"
#include "impl/Stack.hpp"
#include "utils/AtScopeExit.hpp"

namespace smartlua
{

class Value
{
public:
	Value() = delete;

	explicit Value(impl::Reference && ref_):
		ref(ref_)
	{ }

	void push(lua_State * state) const { ref.push(); }

	template<class T>
	T get() const
	{
		AtScopeExit(lua_pop(ref.getState(), 1));
		ref.push();
		return impl::Stack<T>::get(ref.getState(), -1);
	}

	template<class T>
	T isTypeOf() const
	{
		AtScopeExit(lua_pop(ref.getState(), 1));
		ref.push();
		return impl::Stack<T>::is(ref.getState(), -1);
	}

	template<class T>
	std::tuple<T, Error> safeGet() const
	{
		AtScopeExit(lua_pop(ref.getState(), 1));
		ref.push();
		return impl::Stack<T>::safeGet(ref.getState(), -1);
	}

private:
	impl::Reference ref;
};

}
