#include "CircuitScriptParser.h"

#include "ParseException.h"

std::optional<CircuitScriptTokenInfo> CircuitScriptParser::eatToken(const CircuitScriptTokenKind kind)
{
	if (lookahead.tokenKind == kind)
	{
		return eatToken();
	}
	throw ParseException("ParseError: Token(" + TokenKindToString(kind) + ") expected");
}

std::optional<CircuitScriptTokenInfo> CircuitScriptParser::eatToken()
{
	if (const auto optional = lexer.nextToken(); optional.has_value())
	{
		auto old = lookahead;
		lookahead = optional.value();
		return old;
	}
	hasNextToken = false;
	return {};
}

void CircuitScriptParser::program()
{
	declOrConnList();
}

void CircuitScriptParser::declOrConnList()
{
	while (hasNextToken && lookahead.tokenKind == CircuitScriptTokenKind::Identifier || lookahead.tokenKind == CircuitScriptTokenKind::KeywordConnect)
	{
		declOrConn(); 
	}
}

void CircuitScriptParser::declOrConn()
{
	switch (lookahead.tokenKind)
	{
	case CircuitScriptTokenKind::Identifier:
		decl();
		break;
	case CircuitScriptTokenKind::KeywordConnect:
		conn();
		break;
	case CircuitScriptTokenKind::KeywordPower:
	case CircuitScriptTokenKind::KeywordResistor:
	case CircuitScriptTokenKind::KeywordCapacitor:
	case CircuitScriptTokenKind::KeywordInductor:
	case CircuitScriptTokenKind::Equal:
	case CircuitScriptTokenKind::LeftParen:
	case CircuitScriptTokenKind::RightParen:
	case CircuitScriptTokenKind::Comma:
	case CircuitScriptTokenKind::Number:
	case CircuitScriptTokenKind::KeywordGround:
		throw ParseException("ParseError: Token(Identifier|KeywordConnect) expected");
	}
}

void CircuitScriptParser::decl()
{
	const auto identifier = eatToken(CircuitScriptTokenKind::Identifier);
	eatToken(CircuitScriptTokenKind::Equal);
	const auto unitToken = unit();
	eatToken(CircuitScriptTokenKind::LeftParen);
	const auto parameters = optParam();
	eatToken(CircuitScriptTokenKind::RightParen);
	switch (unitToken.tokenKind)
	{
	case CircuitScriptTokenKind::KeywordCapacitor:
		if (parameters.size() == 1)
		{
			symbolTable.emplace(identifier->text, Node<std::shared_ptr<CircuitScriptGraphNode>>(std::make_shared<CircuitScriptCapacitorGraphNode>(parameters[0], identifier->text), nodeIndex++));
		}
		else
		{
			throw ParseException("ParseError: Constructor \"capacitor\" takes exactly one parameter");
		}
		break;
	case CircuitScriptTokenKind::KeywordInductor:
		if (parameters.size() == 1)
		{
			symbolTable.emplace(identifier->text, Node<std::shared_ptr<CircuitScriptGraphNode>>(std::make_shared<CircuitScriptInductorGraphNode>(parameters[0], identifier->text), nodeIndex++));
		}
		else
		{
			throw ParseException("ParseError: Constructor \"inductor\" takes exactly one parameter");
		}
		break;
	case CircuitScriptTokenKind::KeywordPower:
		if (parameters.size() == 2)
		{
			if (std::ranges::find_if(symbolTable, [](const std::pair<std::string, Node<std::shared_ptr<CircuitScriptGraphNode>>>& pair) { return pair.second.data->kind == CircuitScriptGraphNodeKind::Power; }) != symbolTable.end())
			{
				throw ParseException("ParseError: Only one power unit can be defined");
			}
			symbolTable.emplace(identifier->text, Node<std::shared_ptr<CircuitScriptGraphNode>>(std::make_shared<CircuitScriptPowerGraphNode>(parameters[0], parameters[1], identifier->text), 0));
		}
		else
		{
			throw ParseException("ParseError: Constructor \"power\" takes exactly two parameters");
		}
		break;
	case CircuitScriptTokenKind::KeywordResistor:
		if (parameters.size() == 1)
		{
			symbolTable.emplace(identifier->text, Node<std::shared_ptr<CircuitScriptGraphNode>>(std::make_shared<CircuitScriptResistorGraphNode>(parameters[0], identifier->text), nodeIndex++));
		}
		else
		{
			throw ParseException("ParseError: Constructor \"resistor\" takes exactly one parameter");
		}
		break;
	case CircuitScriptTokenKind::KeywordGround:
		if (parameters.empty())
		{
			symbolTable.emplace(identifier->text, Node<std::shared_ptr<CircuitScriptGraphNode>>(std::make_shared<CircuitScriptGroundGraphNode>(identifier->text), nodeIndex++));
		}
		else
		{
			throw ParseException("ParseError: Constructor \"ground\" takes no parameter");
		}
		break;
	case CircuitScriptTokenKind::KeywordConnect:
	case CircuitScriptTokenKind::Equal:
	case CircuitScriptTokenKind::LeftParen:
	case CircuitScriptTokenKind::RightParen:
	case CircuitScriptTokenKind::Comma:
	case CircuitScriptTokenKind::Identifier:
	case CircuitScriptTokenKind::Number:
		break;
	}
}

void CircuitScriptParser::conn()
{
	eatToken(CircuitScriptTokenKind::KeywordConnect);
	eatToken(CircuitScriptTokenKind::LeftParen);
	const auto from = eatToken(CircuitScriptTokenKind::Identifier);
	eatToken(CircuitScriptTokenKind::Comma);
	const auto to = eatToken(CircuitScriptTokenKind::Identifier);
	eatToken(CircuitScriptTokenKind::RightParen);
	if (!symbolTable.contains(from->text))
	{
		throw ParseException("ParseError: Undefined unit used: " + from->text);
	}
	if (!symbolTable.contains(to->text))
	{
		throw ParseException("ParseError: Undefined unit used: " + to->text);
	}
	graph.addEdge(symbolTable.at(from->text), symbolTable.at(to->text));
}

std::vector<double> CircuitScriptParser::optParam()
{
	if (lookahead.tokenKind == CircuitScriptTokenKind::Number)
	{
		return paramList();
	}
	return {};
}

std::vector<double> CircuitScriptParser::paramList()
{
	const auto first = std::stod(eatToken(CircuitScriptTokenKind::Number)->text);
	auto rest = paramListRest();
	rest.insert(rest.begin(), first);
	return rest;
}

std::vector<double> CircuitScriptParser::paramListRest()
{
	std::vector<double> params;
	while (lookahead.tokenKind == CircuitScriptTokenKind::Comma)
	{
		eatToken();
		params.push_back(std::stod(eatToken(CircuitScriptTokenKind::Number)->text));
	}
	return params;
}

CircuitScriptTokenInfo CircuitScriptParser::unit()
{
	if (lookahead.tokenKind == CircuitScriptTokenKind::KeywordCapacitor ||
		lookahead.tokenKind == CircuitScriptTokenKind::KeywordInductor ||
		lookahead.tokenKind == CircuitScriptTokenKind::KeywordPower ||
		lookahead.tokenKind == CircuitScriptTokenKind::KeywordResistor ||
		lookahead.tokenKind == CircuitScriptTokenKind::KeywordGround)
	{
		return eatToken().value();
	}
	throw ParseException("ParseError: Token(KeywordCapacitor|KeywordInductor|KeywordPower|KeywordResistor) expected");
}

CircuitScriptParser::CircuitScriptParser(CircuitScriptLexer lexer) : nodeIndex(1), hasNextToken(true), lexer(std::move(lexer))
{
	if (const auto firstToken = this->lexer.nextToken(); firstToken.has_value())
	{
		lookahead = firstToken.value();
	}
	else
	{
		throw ParseException("ParseError: Expecting token");
	}
}

Graph<std::shared_ptr<CircuitScriptGraphNode>> CircuitScriptParser::parse()
{
	program();
	return graph;
}
