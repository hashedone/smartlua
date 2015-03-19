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
#include "Error.hpp"

#include <boost/format.hpp>

#include <vector>
#include <tuple>

namespace smartlua
{

/*namespace impl
{

template<int N, class Tuple>
struct ExtractResults
{
	static Error get(lua_State * state, Tuple & result, const std::string & name)
	{
		smartlua::Stack stack(state);
		auto e = stack.safeGet(std::get<N-1>(result), N);
		if(!e)
		{
			return Error::stackError(
				"function " + name,
				(boost::format("extracting result %1%") % N).str(),
				e.desc);
		}

		return ExtractResults<N-1, Tuple>::get(state, result, name);
	}
};

template<class Tuple>
struct ExtractResults<0, Tuple>
{
	static Error get(lua_State *, Tuple &, const std::string &) { return Error::noError(); }
};

}

template<class... Rs>
struct MultiReturn { };*/

/**
 * Function returning any number of arguments with the same type
 */
/*template<class R>
class Function<MultiReturn<R>>
{
public:
	Function(impl::Reference && ref, const std::string & name = "__UNKNOWN_FUNCTION__"):
		lastError(Error::noError()),
		fnc(std::move(ref), name, lastError)
	{ }

	Error error() { return lastError; }
	operator bool() const { return fnc; }

	template<class... Args>
	std::vector<R> operator()(Args... args)
	{
		lua_State * state;
		std::tie(state, lastError) = fnc(LUA_MULTRET, args...);
		Stack stack(state);
		if(!lastError)
		{
			stack.size(0);
			return std::vector<R>();
		}

		std::vector<R> result;
		auto it = std::inserter(result, result.end());
		for(int i = 1; i <= stack.size(); ++i)
		{
			lastError = stack.safeGet(it, i);
			if(!lastError)
			{
				lastError = Error::stackError(
					"function " + fnc.getName(),
					(boost::format("extracting result %1%") % i).str(),
					lastError.desc);
				stack.size(0);
				return result;
			}
		}

		stack.size(0);
		return result;
	}

private:
	Error lastError;
	impl::Function fnc;
};

template<class... Rs>
class Function<MultiReturn<Rs...>>
{
public:
	Function(impl::Reference && ref, const std::string & name = "__UNKNOWN_FUNCTION__"):
		lastError(Error::noError()),
		fnc(std::move(ref), name, lastError)
	{ }

	Error error() { return lastError; }
	operator bool() const { return fnc; }

	template<class... Args>
	std::tuple<Rs...> operator()(Args... args)
	{
		lua_State * state;
		std::tie(state, lastError) = fnc(sizeof...(Rs), args...);
		Stack stack(state);
		if(!lastError)
		{
			stack.size(0);
			return std::tuple<Rs...>();
		}

		std::tuple<Rs...> result;
		lastError = impl::ExtractResults<sizeof...(Rs), std::tuple<Rs...>>::get(state, result, fnc.getName());
		stack.size(0);
		return result;
	}

private:
	Error lastError;
	impl::Function fnc;
};*/

}
