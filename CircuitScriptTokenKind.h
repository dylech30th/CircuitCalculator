// GPL v3 License
// 
// CircuitCalculator/CircuitCalculator
// Copyright (c) 2022 CircuitCalculator/CircuitScriptTokenKind.h
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

enum class CircuitScriptTokenKind
{
	KeywordPower,
	KeywordResistor,
	KeywordCapacitor,
	KeywordInductor,
	KeywordGround,
	KeywordConnect,
	Equal,
	LeftParen,
	RightParen,
	Comma,
	Identifier,
	Number
};

inline std::string TokenKindToString(const CircuitScriptTokenKind& kind)
{
	switch (kind)
	{
	case CircuitScriptTokenKind::KeywordPower: return "KeywordPower";
	case CircuitScriptTokenKind::KeywordResistor: return "KeywordResistor";
	case CircuitScriptTokenKind::KeywordCapacitor: return "KeywordCapacitor";
	case CircuitScriptTokenKind::KeywordInductor: return "KeywordInductor";
	case CircuitScriptTokenKind::KeywordConnect: return "KeywordConnect";
	case CircuitScriptTokenKind::KeywordGround: return "KeywordGround";
	case CircuitScriptTokenKind::Equal: return "=";
	case CircuitScriptTokenKind::LeftParen: return "(";
	case CircuitScriptTokenKind::RightParen: return ")";
	case CircuitScriptTokenKind::Comma: return ",";
	case CircuitScriptTokenKind::Identifier: return "Identifier";
	case CircuitScriptTokenKind::Number: return "Number";
	}
	return "";
}
