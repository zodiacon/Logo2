// Lego.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <Tokenizer.h>
#include "Parser.h"
#include "Logo2Ast.h"
#include "Interpreter.h"
#include <print>

int main() {
	using namespace std;

	Tokenizer t;
	Parser parser(t);

	try {
		auto code = R"(
		var a=2*3; 
		var b=a+4;
		var k =true;
		const c = 12 * a;
		a=b+1;
		6+b*a
		)"sv;

		t.Tokenize(code);

		auto ast = parser.Parse(code);

		std::println("{}", ast->ToString());
		Interpreter inter;
		try {
			auto result = ast->Accept(&inter);
			std::println("result: {}", result.ToString());
		}
		catch (RuntimeError const& err) {
			printf("Error! %d\n", (int)err.Error);
		}
	}
	catch (ParserException const& ex) {
		printf("Parse Error: %d (%d,%d)\n", ex.Error, ex.ErrorToken.Line, ex.ErrorToken.Col);
	}

	return 0;
}
