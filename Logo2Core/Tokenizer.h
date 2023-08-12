#pragma once

#include "Token.h"

class Tokenizer {
public:
	bool Tokenize(std::string_view text, int line = 1);

	bool AddToken(std::string lexeme, TokenType type);
	bool AddTokens(std::span<std::pair<std::string, TokenType>> tokens);

	Token Next();
	Token Peek();

private:
	void EatWhitespace();
	Token ParseIdentifier();
	Token ParseNumber();
	Token ParseOperator();

	int m_Line, m_Col{ 1 };
	std::unordered_map<std::string, TokenType> m_TokenTypes;
	std::string_view m_Text;
	const char* m_End;
	const char* m_Current{ nullptr };
};

