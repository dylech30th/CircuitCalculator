// GPL v3 License
// 
// CircuitCalculator/CircuitCalculator
// Copyright (c) 2022 CircuitCalculator/CircuitGraphEvaluator.h
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
#include <memory>
#include <optional>

#include "CircuitScriptGraphNode.h"
#include "Graph.h"

class CircuitGraphEvaluator
{
	Graph<std::shared_ptr<CircuitScriptGraphNode>> graph;

	Graph<std::string> reducedGraph;

	std::string impedance(const std::shared_ptr<CircuitScriptGraphNode>&);

	std::string generateSerialEquation(const std::vector<Node<std::string>>& set);

	static std::string generateParallelEquation(const std::vector<std::string>& vec);

	void translateGraph();

	std::unordered_map<std::pair<Node<std::string>, Node<std::string>>, std::vector<std::vector<Node<std::string>>>> allNonTrivialPathsOfReducedGraph();

	template <typename T>
	static std::optional<std::pair<std::pair<Node<T>, Node<T>>, std::vector<std::vector<Node<T>>>>> anyNonBranchingParallelEdge(Graph<T>& graph, const std::unordered_map<std::pair<Node<T>, Node<T>>, std::vector<std::vector<Node<T>>>>& allPaths);

	bool reduce();
public:
	explicit CircuitGraphEvaluator(const Graph<std::shared_ptr<CircuitScriptGraphNode>>& graph);

	std::string generateEquation();
};