// GPL v3 License
// 
// CircuitCalculator/CircuitCalculator
// Copyright (c) 2022 CircuitCalculator/CircuitScriptParser.h
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

#include "CircuitScriptGraphNode.h"
#include "CircuitScriptLexer.h"
#include "Graph.h"

// The recursive descent parser of the circuit expression
class CircuitScriptParser
{
	int nodeIndex;
	bool hasNextToken;
	CircuitScriptLexer lexer;
	Graph<std::shared_ptr<CircuitScriptGraphNode>> graph;
	CircuitScriptTokenInfo lookahead;
	std::unordered_map<std::string, Node<std::shared_ptr<CircuitScriptGraphNode>>> symbolTable;

	std::optional<CircuitScriptTokenInfo> eatToken(CircuitScriptTokenKind kind);

	std::optional<CircuitScriptTokenInfo> eatToken();


	void program();

	void declOrConnList();

	void declOrConn();

	void decl();

	void conn();

	std::vector<double> optParam();

	std::vector<double> paramList();

	std::vector<double> paramListRest();

	CircuitScriptTokenInfo unit();
public:
	explicit CircuitScriptParser(CircuitScriptLexer lexer);

	Graph<std::shared_ptr<CircuitScriptGraphNode>> parse();
};
