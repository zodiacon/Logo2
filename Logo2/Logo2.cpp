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
#include <Runtime.h>
#include <conio.h>

const char* TokenTypeToString(Logo2::TokenType type) {
	switch (type) {
		case Logo2::TokenType::Integer: return "Integer";
		case Logo2::TokenType::Real: return "Real";
		case Logo2::TokenType::Identifier: return "Identifier";
		case Logo2::TokenType::String: return "String";
	}
	return "";
}

int main() {
	using namespace std;
	using namespace Logo2;

	Tokenizer t;
	Parser parser(t);
	Interpreter inter;
	Runtime runtime(inter);
	runtime.Init();
	runtime.CreateLogoWindow(L"Logo 2", 800, 800);

	for (;;) {
		std::print(">> ");
		std::string input;
		int ch = 0;
		while (ch != 13) {
			MSG msg;
			while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			if (msg.message == WM_QUIT)
				break;

			if (_kbhit()) {
				ch = _getch();
				if (isprint(ch)) {
					input += (char)ch;
					printf("%c", ch);
				}
				else if (ch == 8) {		// backspace
					printf("\b \b");
					input = input.substr(0, input.length() - 1);
				}
				else {
					if (_kbhit())
						_getch();
				}
			}
		}

		try {
			printf("\n");
			auto ast = parser.Parse(input);
			if (parser.HasErrors()) {
				for (auto& err : parser.Errors()) {
					printf("Error (%d,%d): %d\n", err.ErrorToken.Line, err.ErrorToken.Col, err.Error);
				}
				continue;
			}
			try {
				auto result = ast->Accept(&inter);
				if (result != nullptr)
					std::println("{}", result.ToString());
			}
			catch (RuntimeError const& err) {
				printf("Runtime error: %d\n", (int)err.Error);
			}
		}
		catch (ParserError const& err) {
			printf("Error (%d,%d): %d\n", err.ErrorToken.Line, err.ErrorToken.Col, err.Error);
			continue;
		}

	}

	return 0;
}
