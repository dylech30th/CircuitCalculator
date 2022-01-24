// GPL v3 License
// 
// CircuitCalculator/CircuitCalculator
// Copyright (c) 2022 CircuitCalculator/ElementaryCircuits.h
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

#include "Graph.h"
#include "StrongComponents.h"

// Find all elementary circuits in a graph, where "elementary" means no node can occur more than one times in a loop
// based on the research of Donald. B. Johnson, for the correctness proof, complexity analysis, and the original paper
// itself, see https://www.cs.tufts.edu/comp/150GA/homeworks/hw1/Johnson%2075.PDF
template <typename T>
class ElementaryCircuits
{
	Graph<T> graph;
	std::vector<Node<T>> stack;
	std::unordered_map<Node<T>, std::unordered_set<Node<T>>> blockMap;
	std::unordered_set<Node<T>> blocked;
	std::vector<std::set<Node<T>>> result;
	int s;

	void unblock(const Node<T>& node)
	{
		for (const Node<T>& n : blockMap[node] | std::views::filter([this](const Node<T>& it) { return blocked.contains(it); }))
		{
			unblock(n);
		}
		blocked.erase(node);
	}

	bool circuit(const Node<T>& node)
	{
		auto find = false;
		stack.push_back(node);
		blocked.insert(node);

		for (const Node<T>& successor : graph.adjacencyList[node])
		{
			if (successor.index == s)
			{
				std::set<Node<T>> set;
				std::ranges::transform(stack, std::inserter(set, set.begin()), std::identity{});
				result.push_back(set);
				find = true;
			}
			else if (!blocked.contains(successor))
			{
				if (circuit(successor))
				{
					find = true;
				}
			}
		}

		if (find)
		{
			unblock(node);
		}
		else
		{
			for (const Node<T>& n : graph.adjacencyList[node])
			{
				blockMap[n].insert(node);
			}
		}
		stack.pop_back();
		return find;
	}
public:
	explicit ElementaryCircuits(const Graph<T>& graph) : graph(graph), s(0)
	{
		for (auto [vertex, _] : graph.adjacencyList)
		{
			blockMap[vertex] = std::unordered_set<Node<T>>();
		}
	}

	std::vector<std::set<Node<T>>> elementaryCircuits()
	{
		s = graph.vertices().begin()->index;

		const auto sizePredicate = [](const std::set<Node<T>>& set) { return set.size() > 1; };
		const auto size = static_cast<int>(graph.vertices().size());
		while (s < size)
		{
			if (auto subGraphStrongComponents = StrongComponents(graph.subGraph(s)).strongComponents();
				!subGraphStrongComponents.empty() && std::ranges::find_if(subGraphStrongComponents, sizePredicate) != subGraphStrongComponents.end())
			{
				std::vector<std::set<Node<T>>> sortedFilteredNodes;
				for (const std::set<Node<T>>& subGraphStrongComponent : subGraphStrongComponents)
				{
					if (subGraphStrongComponent.size() > 1)
					{
						sortedFilteredNodes.push_back(subGraphStrongComponent);
					}
				}
				std::sort(sortedFilteredNodes.begin(), sortedFilteredNodes.end(), [](const std::set<Node<T>>& lhs, const std::set<Node<T>>& rhs) { return lhs.begin()->index < rhs.begin()->index; });
				std::set<Node<T>> leastIndexNodes = *sortedFilteredNodes.begin();
				auto leastIndexNode = leastIndexNodes.begin();
				s = leastIndexNode->index;
				for (const Node<T>& n : leastIndexNodes)
				{
					blocked.erase(n);
					blockMap[n].clear();
				}
				circuit(*leastIndexNode);
				s++;
			}
			else
			{
				break;
			}
		}
		return result;
	}
};
