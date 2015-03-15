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

#include <type_traits>
#include <utility>
#include <tuple>
#include <array>

namespace smartlua { namespace utils
{

template<class T, class E=void>
struct is_luatable_type: std::false_type { };

template<class T>
struct is_luatable_type<T, typename std::enable_if<
	!std::is_void<typename T::value_type>::value &&
	!std::is_void<decltype(std::declval<T&>().begin())>::value &&
	!std::is_void<decltype(std::declval<T&>().end())>::value
>::type>: std::true_type { };

template<class... Args>
struct is_luatable_type<std::tuple<Args...>>: std::true_type { };

template<class T, int N>
struct is_luatable_type<std::array<T, N>>: std::true_type { };

} }
