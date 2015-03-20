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

#include <string>

namespace smartlua
{

struct Error
{
	enum class Code
	{
		Ok,
		BadReferenceType,
		EmptyReferenceUsage,
		RuntimeError,
		BadType
	} code;

	std::string desc;

	operator bool() const { return code == Code::Ok; }

	static Error noError() { return Error { Code::Ok, "" }; }

	static Error badReference(const std::string & expected, const std::string & found)
	{
		return Error { Code::BadReferenceType, "bad reference type: expected " + expected + ", " + found + " found" };
	}

	static Error emptyReferenceUsage(const std::string & purpose)
	{
		return Error { Code::EmptyReferenceUsage, "empty reference usage (" + purpose + ")" };
	}

	static Error runtimeError(const std::string & error)
	{
		return Error { Code::RuntimeError, "runtime error: " + error };
	}

	static Error badType(std::string const & expected, std::string const & found)
	{
		return { Code::BadType, "bad type: expected " + expected + ", " + found + " found" };
	}
};

}
