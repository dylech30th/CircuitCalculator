// GPL v3 License
// 
// CircuitCalculator/CircuitCalculator
// Copyright (c) 2022 CircuitCalculator/CircuitScriptTokenInfo.h
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

#include "CircuitScriptTokenKind.h"

class CircuitScriptTokenInfo
{
public:
	CircuitScriptTokenKind tokenKind;
	std::string text;

	// compiler compliant
	CircuitScriptTokenInfo() = default;

	CircuitScriptTokenInfo(const CircuitScriptTokenKind tokenKind, std::string text) : tokenKind(tokenKind), text(std::move(text))
	{
	}
};
