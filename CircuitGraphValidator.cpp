#include "CircuitGraphValidator.h"

#include <algorithm>
#include <numeric>

#include "CircuitExceptions.h"
#include "ElementaryCircuits.h"

void CircuitGraphValidator::validate()
{
	if (graph.vertices().begin()->data->kind != CircuitScriptGraphNodeKind::Power)
	{
		
	}
	// Check if all units are reachable from the power supply
	auto allVertices = graph.vertices();
	auto reachableFromPower = graph.reachable(*graph.vertices().begin());
	std::set<Node<std::shared_ptr<CircuitScriptGraphNode>>> diff;
	std::ranges::set_difference(allVertices, reachableFromPower, std::inserter(diff, diff.begin()));
	if (!diff.empty())
	{
		throw UnreachableUnitException(diff);
	}
	// Check if all units are in the circuits
	ElementaryCircuits ec(graph);
	auto elementaryCircuits = ec.elementaryCircuits();
	auto allCircuits = std::accumulate(elementaryCircuits.begin(), elementaryCircuits.end(), std::set<Node<std::shared_ptr<CircuitScriptGraphNode>>>(),
		[](std::set<Node<std::shared_ptr<CircuitScriptGraphNode>>> accumulator, const std::set<Node<std::shared_ptr<CircuitScriptGraphNode>>>& current)
		{
			for (auto& node : current)
			{
				accumulator.insert(node);
			}
			return accumulator;
		});
	std::set<Node<std::shared_ptr<CircuitScriptGraphNode>>> circuitDiff;
	std::ranges::set_difference(allVertices, allCircuits, std::inserter(circuitDiff, circuitDiff.begin()));
	if (!circuitDiff.empty())
	{
		throw UnitPartiallyConnectedException(circuitDiff);
	}
}
