// Lego.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <Tokenizer.h>
#include <Token.h>
#include <Parser.h>
#include "Logo2Ast.h"
#include "Interpreter.h"
#include <print>
#include <Errors.h>

#pragma comment(lib, "SDL3")

const char* TokenTypeToString(TokenType type) {
	switch (type) {
		case TokenType::Integer: return "Integer";
		case TokenType::Real: return "Real";
		case TokenType::Identifier: return "Identifier";
		case TokenType::String: return "String";
	}
	return "";
}

int main() {
	using namespace std;

	Tokenizer t;
	Parser parser(t);

	try {
		auto code = R"(
		var a=2*3; 
		var b=a+4;
		var k =true;
		//const c = 12 * a;
		a=b+1;
		fd(40);
		6+b*a
		)";

		t.Tokenize(code);
		for (auto next = t.Next(); next.Type != TokenType::Invalid; next = t.Next()) {
			printf("(%d,%d): %s (%s)\n", next.Line, next.Col, next.Lexeme.c_str(), TokenTypeToString(next.Type));
		}

		auto ast = parser.Parse(code);
		std::println("{}", ast->ToString());
		if (parser.HasErrors()) {
			for (auto& err : parser.Errors()) {
				printf("(%d,%d): %d\n", err.ErrorToken.Line, err.ErrorToken.Col, err.Error);
			}
		}
		else {
			Runtime rt;
			Interpreter inter(rt);
			try {
				auto result = ast->Accept(&inter);
				std::println("result: {}", result.ToString());
			}
			catch (RuntimeError const& err) {
				printf("Error! %d\n", (int)err.Error);
			}
			while (rt.PumpMessages()) {
				rt.Draw();
			}
		}
	}
	catch (ParserError const& ex) {
		printf("Parse Error: %d (%d,%d)\n", ex.Error, ex.ErrorToken.Line, ex.ErrorToken.Col);
	}

	return 0;
}
