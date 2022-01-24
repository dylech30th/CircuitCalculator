#include <iostream>

#include "CircuitGraphEvaluator.h"
#include "CircuitGraphValidator.h"
#include "CircuitScriptLexer.h"
#include "CircuitScriptParser.h"
#include "Graph.h"

int main()
{
	/*CircuitScriptLexer lexer(R""""(
u0 = power(10, 50)
u1 = resistor(4.7)
u2 = resistor(4.8)
u3 = capacitor(470)
u4 = resistor(5.0)
u5 = inductor(50)
u6 = resistor(5.2)
u7 = inductor(60)
u8 = resistor(5.3)
u9 = ground()

connect(u0, u1)
connect(u1, u3)
connect(u1, u4)
connect(u3, u7)
connect(u4, u8)
connect(u7, u8)
connect(u8, u9)
connect(u9, u5)
connect(u9, u6)
connect(u5, u2)
connect(u6, u2)
connect(u2, u0))"""");*/
	CircuitScriptLexer lexer(R"(
u2 = power(10, 50)
u3 = resistor(4.7)
u6 = resistor(5)
u4 = resistor(4.8)
u9 = capacitor(100)
u5 = capacitor(470)
u7 = inductor(50)
u8 = resistor(5.2)
u10 = inductor(60)
u11 = resistor(5.3)
connect(u2, u3)
connect(u11, u9)
connect(u3, u6)
connect(u10, u11)
connect(u5, u10)
connect(u6, u11)
connect(u9, u8)
connect(u8, u4)
connect(u7, u4)
connect(u4, u2)
)");
	CircuitScriptParser parser(lexer);
	auto graph = parser.parse();
	CircuitGraphValidator validator(graph);
	validator.validate();
	CircuitGraphEvaluator evaluator(graph);
	std::cout << evaluator.generateEquation() << std::endl;
}