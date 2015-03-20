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

#include "../Error.hpp"
#include "../utils/AtScopeExit.hpp"
#include "Reference.hpp"

#include <string>
#include <vector>
#include <tuple>

namespace smartlua
{

template<class... Returns>
struct MultiplyReturn { };

namespace impl
{

class Function
{
public:
	Function(Reference && ref_):
		ref(ref_)
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
	Reference & getReference() { return ref; }
	Reference const & getReference() const { return ref; }

	template<class... Args>
	Error operator()(lua_State * thread, int retc, Args... args) const
	{
		if(!ref)
		{
			return Error::emptyReferenceUsage("function call");
		}

		ref.push(thread);

		pushArgs(thread, args...);
		if(lua_pcall(thread, sizeof...(Args), retc, 0))
		{
			AtScopeExit(lua_settop(thread, 0));
			return Error::runtimeError(lua_tostring(thread, -1));
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
};

template<class R, class E = void>
struct FunctionTraits
{
	using ReturnType = R;
	static constexpr int returnsCount = 1;

	static std::tuple<R, Error> extractResult(lua_State * state)
	{
		return Stack<R>::safeGet(state, -1);
	}
};

template<class R>
struct FunctionTraits<MultiplyReturn<R>>
{
	using ReturnType = std::vector<R>;
	static constexpr int returnsCount = LUA_MULTRET;

	static std::tuple<std::vector<R>, Error> extractResult(lua_State * state)
	{
		std::vector<R> result;
		int size = lua_gettop(state);
		result.reserve(size);

		Error e = Error::noError();
		auto it = std::inserter(result, result.end());
		for(int i = 1; i <= size && e; ++i)
			std::tie(it, e) = Stack<R>::safeGet(state, i);

		return std::make_tuple(result, e);
	}
};

template<int N, class Tuple>
struct ExtractResults
{
	static Error get(lua_State * state, Tuple & result)
	{
		Error e;
		std::tie(std::get<N-1>(result), e) = Stack<typename std::tuple_element<N-1, Tuple>::type>::safeGet(state, N);
		if(!e) return e;

		return ExtractResults<N-1, Tuple>::get(state, result);
	}
};

template<class Tuple>
struct ExtractResults<0, Tuple>
{
	static Error get(lua_State *, Tuple &) { return Error::noError(); }
};

template<class... Results>
struct FunctionTraits<MultiplyReturn<Results...>, typename std::enable_if<(sizeof...(Results) > 1)>::type>
{
	using ReturnType = std::tuple<Results...>;
	static constexpr int returnsCount = sizeof...(Results);

	static std::tuple<std::tuple<Results...>, Error> extractResult(lua_State * state)
	{
		std::tuple<Results...> result;
		auto e = ExtractResults<sizeof...(Results), std::tuple<Results...>>::get(state, result);

		return std::make_tuple(result, e);
	}
};

} }
