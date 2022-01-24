#include "CircuitScriptLexer.h"

#include <sstream>

CircuitScriptTokenInfo CircuitScriptLexer::parseNumber()
{
	std::stringstream ss;
	while (index < static_cast<int>(str.size()) && isNumber(str.at(index)))
	{
		ss << str.at(index++);
	}
	if (index < static_cast<int>(str.size()) && str.at(index) == '.')
	{
		index++;
		ss << '.';
		while (index < static_cast<int>(str.size()) && isNumber(str.at(index)))
		{
			ss << str.at(index++);
		}
	}
	return { CircuitScriptTokenKind::Number, ss.str() };
}

CircuitScriptTokenInfo CircuitScriptLexer::parseKeywordOrIdentifier()
{
	std::stringstream ss;
	while (index < static_cast<int>(str.size()) && (std::isalpha(str.at(index)) || std::isdigit(str.at(index))))
	{
		ss << str.at(index++);
	}

	auto s = ss.str();

	if (keywordDictionary.contains(s))
	{
		return keywordDictionary.at(s);
	}
	return { CircuitScriptTokenKind::Identifier, s };
}

bool CircuitScriptLexer::isNumber(const char n)
{
	return n >= '0' && n <= '9';
}

CircuitScriptLexer::CircuitScriptLexer(std::string str) : str(std::move(str))
{
	keywordDictionary.emplace("connect", CircuitScriptTokenInfo(CircuitScriptTokenKind::KeywordConnect, "connect"));
	keywordDictionary.emplace("power", CircuitScriptTokenInfo(CircuitScriptTokenKind::KeywordPower, "power"));
	keywordDictionary.emplace("resistor", CircuitScriptTokenInfo(CircuitScriptTokenKind::KeywordResistor, "resistor"));
	keywordDictionary.emplace("capacitor", CircuitScriptTokenInfo(CircuitScriptTokenKind::KeywordCapacitor, "capacitor"));
	keywordDictionary.emplace("ground", CircuitScriptTokenInfo(CircuitScriptTokenKind::KeywordGround, "ground"));
	keywordDictionary.emplace("inductor", CircuitScriptTokenInfo(CircuitScriptTokenKind::KeywordInductor, "inductor"));
}

std::optional<CircuitScriptTokenInfo> CircuitScriptLexer::nextToken()
{
	auto skipBlanks = [this]
	{
		while (index < static_cast<int>(str.size()) && (str.at(index) == '\r' || str.at(index) == '\n' || str.at(index) == '\t' || str.at(index) == ' '))
		{
			index++;
		}
	};
	if (index >= static_cast<int>(str.size()))
	{
		return {};
	}

	switch (str.at(index))
	{
	case '=':
		index++;
		return CircuitScriptTokenInfo(CircuitScriptTokenKind::Equal, "=");
	case '(':
		index++;
		return CircuitScriptTokenInfo(CircuitScriptTokenKind::LeftParen, "(");
	case ')':
		index++;
		return CircuitScriptTokenInfo(CircuitScriptTokenKind::RightParen, ")");
	case ',':
		index++;
		return CircuitScriptTokenInfo(CircuitScriptTokenKind::Comma, ",");
	case ' ': 
	case '\n':
	case '\t':
	case '\r':
		skipBlanks();
		return nextToken();
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return parseNumber();
	default:
		return parseKeywordOrIdentifier();
	}
}
