// GPL v3 License
// 
// CircuitCalculator/CircuitCalculator
// Copyright (c) 2022 CircuitCalculator/CircuitScriptAST.h
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
#include <string>

enum class CircuitScriptGraphNodeKind
{
	Power,
	Resistor,
	Capacitor,
	Ground,
	Inductor
};

class CircuitScriptGraphNode
{
public:
	CircuitScriptGraphNodeKind kind;
	std::string tag;

	CircuitScriptGraphNode(const CircuitScriptGraphNodeKind kind, std::string tag) : kind(kind), tag(std::move(tag))
	{
	}

	CircuitScriptGraphNode(const CircuitScriptGraphNode& other) = default;

	CircuitScriptGraphNode(CircuitScriptGraphNode&& other) noexcept : kind(other.kind), tag(std::move(other.tag))
	{
	}

	CircuitScriptGraphNode& operator=(const CircuitScriptGraphNode& other)
	{
		if (this == &other)
			return *this;
		kind = other.kind;
		tag = other.tag;
		return *this;
	}

	CircuitScriptGraphNode& operator=(CircuitScriptGraphNode&& other) noexcept
	{
		if (this == &other)
			return *this;
		kind = other.kind;
		tag = other.tag;
		return *this;
	}

	virtual ~CircuitScriptGraphNode() = default;
};

class CircuitScriptPowerGraphNode final : public CircuitScriptGraphNode
{
public:
	double voltageInVolt;
	double frequencyInHz;

	CircuitScriptPowerGraphNode(const double voltageInVolt, const double frequencyInHz, const std::string& t)
		: CircuitScriptGraphNode(CircuitScriptGraphNodeKind::Power, t), voltageInVolt(voltageInVolt), frequencyInHz(frequencyInHz)
	{
	}
};

class CircuitScriptResistorGraphNode final : public CircuitScriptGraphNode
{
public:
	double resistanceInO;

	explicit CircuitScriptResistorGraphNode(const double resistanceInO, const std::string& t)
		: CircuitScriptGraphNode(CircuitScriptGraphNodeKind::Resistor, t), resistanceInO(resistanceInO)
	{
	}
};

class CircuitScriptCapacitorGraphNode final : public CircuitScriptGraphNode
{
public:
	double capacitanceInF;

	explicit CircuitScriptCapacitorGraphNode(const double capacitanceInF, const std::string& t)
		: CircuitScriptGraphNode(CircuitScriptGraphNodeKind::Capacitor, t), capacitanceInF(capacitanceInF)
	{
	}
};

class CircuitScriptInductorGraphNode final : public CircuitScriptGraphNode
{
public:
	double inductanceInH;

	explicit CircuitScriptInductorGraphNode(const double inductanceInH, const std::string& t)
		: CircuitScriptGraphNode(CircuitScriptGraphNodeKind::Inductor, t), inductanceInH(inductanceInH)
	{
	}
};

class CircuitScriptGroundGraphNode final : public CircuitScriptGraphNode
{
public:
	explicit CircuitScriptGroundGraphNode(const std::string& t) : CircuitScriptGraphNode(CircuitScriptGraphNodeKind::Ground, t)
	{
	}
};