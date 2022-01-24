// GPL v3 License
// 
// CircuitCalculator/CircuitCalculator
// Copyright (c) 2022 CircuitCalculator/Utils.h
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
#include <numeric>
#include <string>

namespace CircuitCalculator::Utils
{
	template <typename Itr, typename Fn = std::identity>
	std::string Join(Itr begin, Itr end, const std::string& delimiter, Fn fn = {})
	{
		return std::accumulate(begin, end, std::string(), [&fn](const std::string& accumulator, const auto& str)
			{
				if (fn(str).empty())
				{
					return accumulator;
				}
				return accumulator.empty() ? fn(str) : accumulator + "+" + fn(str);
			});
	}

	template <typename T>
	bool PrefixDistinct(std::vector<std::vector<T>> vec, bool fromEnd = false)
	{
		std::unordered_set<T> elements;
		auto empties = 0;
		for (const auto& container : vec)
		{
			if (container.empty())
			{
				empties++;
				continue;
			}
			elements.insert(fromEnd ? container.back() : container[0]);
		}
		return elements.size() == vec.size() - empties;
	}
}
