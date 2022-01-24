// GPL v3 License
// 
// CircuitCalculator/CircuitCalculator
// Copyright (c) 2022 CircuitCalculator/InvalidCircuitException.h
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
#include <set>

#include "CircuitScriptGraphNode.h"
#include "Node.h"

class UnreachableUnitException final : std::exception
{
public:
	const std::set<Node<std::shared_ptr<CircuitScriptGraphNode>>>& unreachable;

	explicit UnreachableUnitException(const std::set<Node<std::shared_ptr<CircuitScriptGraphNode>>>& unreachable)
		: std::exception("Unreachable units detected in the circuit"), unreachable(unreachable)
	{
	}
};

class UnitPartiallyConnectedException final : std::exception
{
public:
	const std::set<Node<std::shared_ptr<CircuitScriptGraphNode>>>& disconnected;

	explicit UnitPartiallyConnectedException(const std::set<Node<std::shared_ptr<CircuitScriptGraphNode>>>& disconnected)
		: std::exception("Units that are partially connected from the circuit detected"), disconnected(disconnected)
	{
	}
};

class NoPowerSupplyFoundException final : std::exception
{
public:
	NoPowerSupplyFoundException() : std::exception("You must add at least one power supply") {}
};