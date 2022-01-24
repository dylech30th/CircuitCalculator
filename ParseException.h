// GPL v3 License
// 
// CircuitCalculator/CircuitCalculator
// Copyright (c) 2022 CircuitCalculator/ParseException.h
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once
#include <exception>
#include <string>

class ParseException final : public std::exception
{
public:
	ParseException() = default;

	explicit ParseException(char const* message) : std::exception(message)
	{
	}

	explicit ParseException(const std::string& message) : std::exception(message.c_str())
	{
	}

	ParseException(char const* message, const int i) : std::exception(message, i)
	{
	}

	explicit ParseException(exception const& other) : std::exception(other)
	{
	}
};
