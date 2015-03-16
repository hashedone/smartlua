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

namespace smartlua
{

struct Error
{
	enum class Code
	{
		OK,
		BAD_REFERENCE_TYPE,
		EMPTY_REFERENCE_USAGE,
		RUNTIME_ERROR,
		STACK_ERROR
	} code;

	std::string desc;

	operator bool() const { return code == Code::OK; }

	static Error noError() { return Error { Code::OK, "" }; }

	static Error badReference(const std::string & expected, const std::string & found)
	{
		return Error { Code::BAD_REFERENCE_TYPE, "bad reference type: expected " + expected + ", " + found + " found" };
	}
	static Error badReference(const std::string & prefix, const std::string & expected, const std::string & found)
	{
		return Error { Code::BAD_REFERENCE_TYPE, prefix + ": bad reference type: expected " + expected + ", " + found + " found" };
	}

	static Error emptyReferenceUsage()
	{
		return Error { Code::EMPTY_REFERENCE_USAGE, "empty reference usage" };
	}
	static Error emptyReferenceUsage(const std::string & purpose)
	{
		return Error { Code::EMPTY_REFERENCE_USAGE, "empty reference usage (" + purpose + ")" };
	}
	static Error emptyReferenceUsage(const std::string & prefix, const std::string & purpose)
	{
		return Error { Code::EMPTY_REFERENCE_USAGE, prefix + ": empty reference usage (" + purpose + ")" };
	}

	static Error runtimeError(const std::string & error)
	{
		return Error { Code::RUNTIME_ERROR, "runtime error (" + error + ")" };
	}
	static Error runtimeError(const std::string & prefix, const std::string & error)
	{
		return Error { Code::RUNTIME_ERROR, prefix + ": runtime error (" + error + ")" };
	}

	static Error stackError(const std::string & error)
	{
		return Error { Code::STACK_ERROR, "stack error (" + error + ")" };
	}
	static Error stackError(const std::string & prefix, const std::string & error)
	{
		return Error { Code::STACK_ERROR, prefix + ": stack error (" + error + ")" };
	}
	static Error stackError(const std::string & prefix, const std::string & when, const std::string & error)
	{
		return Error { Code::STACK_ERROR, prefix + ": stack error while " + when + " (" + error + ")" };
	}
};

}
