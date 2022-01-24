#include "CircuitGraphEvaluator.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <numbers>
#include <numeric>
#include <ranges>

#include "Utils.h"

template<typename First, typename Second>
struct std::hash<std::pair<First, Second>>
{
	std::size_t operator()(const std::pair<First, Second>& pair) const
	{
		return std::hash<First>{}(pair.first) ^ std::hash<Second>{}(pair.second);
	}
};

std::string CircuitGraphEvaluator::impedance(const std::shared_ptr<CircuitScriptGraphNode>& unit)
{
	std::stringstream ss;
	const auto power = std::dynamic_pointer_cast<CircuitScriptPowerGraphNode>(graph.vertices().begin()->data);
	if (unit->kind == CircuitScriptGraphNodeKind::Power)
	{
		ss << std::fixed << std::setprecision(2) << "-" << power->voltageInVolt << "";
	}
	else if (unit->kind == CircuitScriptGraphNodeKind::Resistor)
	{
		const auto resistor = std::dynamic_pointer_cast<CircuitScriptResistorGraphNode>(unit);
		ss << std::fixed << std::setprecision(2) << resistor->resistanceInO;
	}
	else if (unit->kind == CircuitScriptGraphNodeKind::Capacitor)
	{
		const auto capacitor = std::dynamic_pointer_cast<CircuitScriptCapacitorGraphNode>(unit);
		ss << std::fixed << std::setprecision(2) << "(-j" << 1 / (2 * std::numbers::pi * power->frequencyInHz * capacitor->capacitanceInF) * 1E06 << ")";
	}
	else if (unit->kind == CircuitScriptGraphNodeKind::Inductor)
	{
		const auto inductor = std::dynamic_pointer_cast<CircuitScriptInductorGraphNode>(unit);
		ss << std::fixed << std::setprecision(2) << "j" << 2 * std::numbers::pi * power->frequencyInHz * inductor->inductanceInH * 1E-3;
	}
	return ss.str();
}

void CircuitGraphEvaluator::translateGraph()
{
	Graph<std::string> translatedGraph;
	std::set<Node<std::string>> newGraphNodes;
	for (const auto& vertex : graph.vertices())
	{
		newGraphNodes.insert(Node(impedance(vertex.data), vertex.index));
	}
	for (const auto& vertex : graph.vertices())
	{
		for (const auto& successor : graph.adjacencyList[vertex])
		{
			translatedGraph.addEdge(
				*std::ranges::find_if(newGraphNodes, [&vertex](const auto& node) { return node.index == vertex.index; }),
				*std::ranges::find_if(newGraphNodes, [&successor](const auto& node) { return node.index == successor.index; })
			);
		}
	}
	reducedGraph = translatedGraph;
}

// Get all non trivial paths, i.e., paths those who have intermediate nodes.
// Returns a map whose key is the start and end node, value is all paths between start(exclusive) and end(exclusive).
std::unordered_map<std::pair<Node<std::string>, Node<std::string>>, std::vector<std::vector<Node<std::string>>>> CircuitGraphEvaluator::allNonTrivialPathsOfReducedGraph()
{
	std::unordered_map<std::pair<Node<std::string>, Node<std::string>>, std::vector<std::vector<Node<std::string>>>> result;
	for (const auto& start : reducedGraph.vertices())
	{
		for (const auto& end : reducedGraph.vertices())
		{
			// we don't want to calculate the path from a single node to itself.
			if (start != end)
			{
				if (const auto& paths = reducedGraph.nonTrivialPaths(start, end); !paths.empty())
				{
					result.emplace(std::make_pair(start, end), paths);
				}
			}
		}
	}
	return result;
}

// Try get any direct paths (i.e., paths that have no branching and are pairwise disjoint) between two arbitrarily selected nodes.
// Returns a map whose key is the start and end node, value is the direct paths between start(exclusive) and end(exclusive)
template <typename T>
std::optional<std::pair<std::pair<Node<T>, Node<T>>, std::vector<std::vector<Node<T>>>>> CircuitGraphEvaluator::anyNonBranchingParallelEdge(
	Graph<T>& graph, 
	const std::unordered_map<std::pair<Node<T>, Node<T>>, std::vector<std::vector<Node<T>>>>& allPaths)
{
	for (const auto& pair : allPaths)
	{
		std::vector<std::vector<Node<T>>> vector;
		for (const auto& vec : pair.second)
		{
			// if the successor (adjacencyList[node]) is more than one, then there is a branching during the path
			if (std::ranges::all_of(vec, [&graph](const Node<T>& node) { return graph.adjacencyList[node].size() == 1; }))
			{
				vector.push_back(vec);
			}
		}
		// we consider only those who have distinct paths (i.e., the paths between [start] and [end] are disjoint)
		if (vector.size() >= 2 && CircuitCalculator::Utils::PrefixDistinct(vector) && CircuitCalculator::Utils::PrefixDistinct(vector, true))
		{
			return std::make_pair(pair.first, vector);
		}
	}
	return {};
}

// This function is the key of the who algorithm
// It iteratively reduce a graph, merge parallel edges until there is only serial paths left in the who graph.
// There is a similar concept called Series-Parallel Graph, see https://en.wikipedia.org/wiki/Series%E2%80%93parallel_graph, the difference here
// is that we consider direct graphs over indirect graphs, and to simulate the circuit the graph will contain a big loop, start and end at the
// node who is denoting the power supply
//
// To reduce a graph, we need to know which part, that is to say, is "parallel", we say path (S, T) and (S', T') are parallel if and only if
// they have no common prefix and common suffix except the start and end node, and they're disjoint
bool CircuitGraphEvaluator::reduce()
{
	const auto allPaths = allNonTrivialPathsOfReducedGraph();
	if (const auto nonBranchingParallelEdges = anyNonBranchingParallelEdge(reducedGraph, allPaths); nonBranchingParallelEdges.has_value())
	{
		std::vector<std::string> serialImpedance;
		const auto& [endPoints, parallelEdges] = nonBranchingParallelEdges.value();
		auto allNodeInParallelEdges = std::accumulate(parallelEdges.begin(), parallelEdges.end(), std::unordered_set<Node<std::string>>(),
			[](std::unordered_set<Node<std::string>> accumulator, const std::vector<Node<std::string>>& set)
			{
				std::ranges::copy(set, std::inserter(accumulator, accumulator.begin()));
				return accumulator;
			});
		// we have guaranteed that all paths in [nonBranchingParallelEdges] is branch-free, which assures that we can simply treat them
		// as serial circuits and add up the impedance of units through the path.
		std::ranges::transform(parallelEdges, std::back_inserter(serialImpedance), [this](const std::vector<Node<std::string>>& set) { return generateSerialEquation(set); });
		// create the node who will replace the nodes in the [allNodeInParallelEdges], it's impedance will be calculated on-the-fly by
		// [generateParallelEquation(serialImpedance)] instead of deferred to the time that the graph has been fully reduced
		Node reducedNode(generateParallelEquation(serialImpedance), std::ranges::max(reducedGraph.vertices()).index + 1);

		// preserve the original topological structure of the graph except those who is the part of the parallel paths, since they are
		// meant to be merged, we don't need them in the new graph
		Graph<std::string> newGraph;
		for (const auto& [start, successors] : reducedGraph.adjacencyList)
		{
			if (!allNodeInParallelEdges.contains(start))
			{
				for (const auto& successor : successors | std::views::filter([&allNodeInParallelEdges](const Node<std::string>& node) { return !allNodeInParallelEdges.contains(node); }))
				{
					newGraph.addEdge(start, successor);
				}
			}
		}

		// relay all the incoming edges that points to any node of parallel path to the new node we've just created
		auto oldVertices = reducedGraph.vertices();
		// ReSharper disable once CppTooWideScopeInitStatement
		auto incomingNodes = oldVertices
			| std::views::filter([&allNodeInParallelEdges](const Node<std::string>& node) { return !allNodeInParallelEdges.contains(node); })
			| std::views::filter([this, &allNodeInParallelEdges](const Node<std::string>& node)
				{
					auto& successors = reducedGraph.adjacencyList[node];
					std::unordered_set<Node<std::string>> dummy;
					std::ranges::set_intersection(successors, allNodeInParallelEdges, std::inserter(dummy, dummy.begin()));
					return dummy.size() > 0;
				});

		for (const auto& incomingNode : incomingNodes)
		{
			newGraph.addEdge(incomingNode, reducedNode);
		}

		// only incoming edges needs to be considered, since we're using a direct graph, and the previous code ensures that no branch
		// will occur in the parallel paths, so the only outgoing edges will be those who pointing to the end of the path.
		newGraph.addEdge(reducedNode, endPoints.second);
		reducedGraph = newGraph;
		return true;
	}
	return false;
}

std::string CircuitGraphEvaluator::generateSerialEquation(const std::vector<Node<std::string>>& set)
{
	return CircuitCalculator::Utils::Join(set.begin(), set.end(), "+", [this](const Node<std::string>& node) { return node.data.empty() ? "" : node.index == graph.vertices().begin()->index ? node.data : node.data + "I"; });
}

inline std::string CircuitGraphEvaluator::generateParallelEquation(const std::vector<std::string>& vec)
{
	auto reversed = vec | std::views::transform([](const std::string& str) { return "[1/" + str + "]"; });
	return "{1/" + CircuitCalculator::Utils::Join(reversed.begin(), reversed.end(), " + ") + "}";
}

std::string CircuitGraphEvaluator::generateEquation()
{
	// reduce the graph until it reaches a fixed point
	do {} while (reduce());
	std::vector<Node<std::string>> vec;
	std::ranges::copy(reducedGraph.vertices(), std::back_inserter(vec));
	return generateSerialEquation(vec);
}

CircuitGraphEvaluator::CircuitGraphEvaluator(const Graph<std::shared_ptr<CircuitScriptGraphNode>>& graph): graph(graph)
{
	translateGraph();
}