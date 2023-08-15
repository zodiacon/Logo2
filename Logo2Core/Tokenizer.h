#pragma once

#include "Token.h"

namespace Logo2 {
	class Tokenizer {
	public:
		bool Tokenize(std::string text, int line = 1);
		void SetCommentToEndOfLine(std::string chars);

		bool AddToken(std::string lexeme, TokenType type);
		bool AddTokens(std::span<std::pair<std::string, TokenType>> tokens);

		Token Next();
		Token Peek();

	private:
		bool ProcessSingleLineComment();
		void EatWhitespace();
		Token ParseIdentifier();
		Token ParseNumber();
		Token ParseOperator();
		Token ParseString();

		int m_Line, m_Col{ 1 };
		std::unordered_map<std::string, TokenType> m_TokenTypes;
		std::string m_Text;
		const char* m_Current{ nullptr };
		std::string m_CommentToEndOfLine{ "//" };
	};
}

