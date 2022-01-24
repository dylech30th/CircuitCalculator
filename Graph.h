// GPL v3 License
// 
// CircuitCalculator/CircuitCalculator
// Copyright (c) 2022 CircuitCalculator/Graph.h
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
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <ranges>
#include <set>

#include "Node.h"

// Represents the topological structure of a graph by adjacency list
template <typename T>
class Graph
{
public:
	std::unordered_map<Node<T>, std::set<Node<T>>> adjacencyList;

	void addEdge(const Node<T>& from, const Node<T>& to)
	{
		if (!adjacencyList.contains(from))
		{
			adjacencyList[from] = std::move(std::set<Node<T>>());
		}
		adjacencyList[from].insert(to);
		if (!adjacencyList.contains(to))
		{
			adjacencyList[to] = std::move(std::set<Node<T>>());
		}
	}

	std::set<Node<T>> vertices()
	{
		std::set<Node<T>> set;
		for (auto node : std::views::keys(adjacencyList))
		{
			set.insert(node);
		}
		return set;
	}

	// Get all non trivial paths (i.e., those who contains more than one intermediate node) between start and node
	std::vector<std::vector<Node<T>>> nonTrivialPaths(const Node<T>& start, const Node<T>& end)
	{
		std::set<Node<T>> visited;
		std::vector<Node<T>> currentPath;
		std::vector<std::vector<Node<T>>> allPaths;

		std::function<void(const Node<T>&, const Node<T>&)> helper;
		helper = [&visited, &currentPath, &allPaths, this, &helper, &start](const Node<T>& s, const Node<T>& e)
		{
			if (visited.contains(s))
			{
				return;
			}
			visited.insert(s);
			currentPath.push_back(s);
			if (s == e)
			{
				std::vector<Node<T>> path;
				std::ranges::copy_if(currentPath, std::inserter(path, path.begin()), [&e, &start](const Node<T>& node) { return node != start && node != e; });
				if (!path.empty())
				{
					allPaths.push_back(path);
				}
				visited.erase(s);
				currentPath.pop_back();
				return;
			}
			for (const auto& successor : adjacencyList[s])
			{
				helper(successor, e);
			}
			currentPath.pop_back();
			visited.erase(s);
		};
		helper(start, end);
		return allPaths;
	}

	// Create a sub graph of current graph, where all the nodes below [leastIndex] is dropped
	Graph<T> subGraph(int leastIndex)
	{
		Graph<T> graph;
		for (const auto& [node, successors] : adjacencyList)
		{
			if (node.index >= leastIndex)
			{
				for (const auto& successor : successors)
				{
					if (successor.index >= leastIndex)
					{
						graph.addEdge(node, successor);
					}
				}
			}
		}
		return graph;
	}

	// Get all nodes that are reachable from given node
	std::set<Node<T>> reachable(const Node<T>& node)
	{
		std::set<Node<T>> visited{ node };
		std::vector<Node<T>> stack{ node };
		while (!stack.empty())
		{
			auto n = stack.back();
			stack.pop_back();
			for (const auto& successor : adjacencyList[n])
			{
				if (!visited.contains(successor))
				{
					stack.push_back(successor);
					visited.insert(successor);
				}
			}
		}
		return visited;
	}
};

template<typename T>
struct std::hash<Node<T>>
{
	std::size_t operator()(const Node<T>& node) const
	{
		return node.index;
	}
};