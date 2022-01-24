// GPL v3 License
// 
// CircuitCalculator/CircuitCalculator
// Copyright (c) 2022 CircuitCalculator/StrongComponents.h
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

#include "Node.h"
#include "Graph.h"

// Tarjan's Algorithm
template <typename T>
class StrongComponents
{
	Graph<T> graph;
	int index;
	std::unordered_map<Node<T>, int> indexMap, lowLinkMap;
	std::vector<Node<T>> stack;
	std::vector<std::set<Node<T>>> result;

	void helper(const Node<T>& node)
	{
		indexMap[node] = index;
		lowLinkMap[node] = index;
		index++;
		stack.push_back(node);
		for (const Node<T>& successor : graph.adjacencyList[node])
		{
			if (!indexMap.contains(successor))
			{
				helper(successor);
				lowLinkMap[node] = std::min(lowLinkMap[node], lowLinkMap[successor]);
			}
			else if (std::ranges::find(stack, successor) != stack.end())
			{
				lowLinkMap[node] = std::min(lowLinkMap[node], indexMap[successor]);
			}
		}

		if (lowLinkMap[node] == indexMap[node])
		{
			std::set<Node<T>> res;
			Node<T> w = stack.back();
			do
			{
				w = stack.back();
				stack.pop_back();
				res.insert(w);
			} while (w.index != node.index);
			result.push_back(res);
		}

	}
public:
	explicit StrongComponents(const Graph<T>& graph) : graph(graph), index(0)
	{
	}

	std::vector<std::set<Node<T>>> strongComponents()
	{
		for (const Node<T>& vertex : graph.vertices())
		{
			if (!indexMap.contains(vertex))
			{
				helper(vertex);
			}
		}
		return result;
	}
};
