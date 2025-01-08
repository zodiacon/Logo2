#include "pch.h"
#include "Tokenizer.h"
#include <assert.h>

using namespace std;

bool Logo2::Tokenizer::Tokenize(string text, int line) {
	m_Text = move(text);
	m_Line = line;
	m_Col = 1;
	m_Current = m_Text.data();
	return true;
}

bool Logo2::Tokenizer::AddToken(string lexeme, TokenType type) {
	return m_TokenTypes.try_emplace(move(lexeme), type).second;
}

int Logo2::Tokenizer::AddTokens(span<pair<string, TokenType>> tokens) {
	auto count = 0;
	for (auto const& [str, type] : tokens)
		count += AddToken(str, type) ? 1 : 0;
	return count;
}

Logo2::Token Logo2::Tokenizer::Next() {
	EatWhitespace();
	auto ch = *m_Current;

	if (isalpha(ch) || ch == '_' || ch == '$') {
		return ParseIdentifier();
	}
	if (ch >= '0' && ch <= '9') {
		return ParseNumber();
	}
	if (ch == '\"') {
		return ParseString();
	}
	return ParseOperator();
}

Logo2::Token Logo2::Tokenizer::Peek() {
	auto current = m_Current;
	auto line = m_Line;
	auto col = m_Col;
	auto token = Next();
	m_Current = current;
	m_Line = line;
	m_Col = col;
	return token;
}

bool Logo2::Tokenizer::ProcessSingleLineComment() {
	auto current = m_Current;
	int i = 0;
	while (*current && *current++ == m_CommentToEndOfLine[i]) {
		if (++i == m_CommentToEndOfLine.length())
			break;
	}
	if (i == m_CommentToEndOfLine.length()) {
		//
		// move to next line
		//
		m_Current = current;
		while (*m_Current && *m_Current != '\n')
			m_Current++;
		m_Current++;
		m_Line++;
		m_Col = 1;
		return true;
	}
	return false;
}

void Logo2::Tokenizer::EatWhitespace() {
	while (*m_Current && isspace(*m_Current)) {
		m_Col++;
		if (*m_Current == '\n') {
			m_Col = 1;
			m_Line++;
		}
		++m_Current;
	}
	if (ProcessSingleLineComment())
		EatWhitespace();
}

Logo2::Token Logo2::Tokenizer::ParseIdentifier() {
	string lexeme;
	while (*m_Current && !isspace(*m_Current) && !ispunct(*m_Current)) {
		if (ProcessSingleLineComment())
			break;
		lexeme += *m_Current++;
		m_Col++;
	}
	assert(!lexeme.empty());
	auto type = TokenType::Identifier;
	if (auto it = m_TokenTypes.find(lexeme); it != m_TokenTypes.end())
		type = it->second;
	int len = (int)lexeme.length();
	return Token{ .Type = type, .Lexeme = move(lexeme), .Line = m_Line, .Col = m_Col - len, };
}

Logo2::Token Logo2::Tokenizer::ParseNumber() {
	char* pd, *pi;
	auto dvalue = strtod(m_Current, &pd);
	auto ivalue = strtoll(m_Current, &pi, 0);
	assert(pd && pi);
	auto type = pd > pi ? TokenType::Real : TokenType::Integer;
	auto len = int(type == TokenType::Real ? pd - m_Current : pi - m_Current);
	m_Col += (int)len;
	m_Current += len;
	auto token = Token{ .Type = type, .Lexeme = string(m_Current - len, m_Current), .Line = m_Line, .Col = m_Col - len };
	if (*m_Current == '\n') {
		m_Col = 1;
		m_Line++;
		m_Current++;
	}
	ProcessSingleLineComment();
	return token;
}

Logo2::Token Logo2::Tokenizer::ParseOperator() {
	string lexeme;
	while (*m_Current && ispunct(*m_Current)) {
		//
		// treat parenthesis as special so they are not combined with other operators
		//
		if (lexeme == "(" || lexeme == ")" || (!lexeme.empty() && (*m_Current == '(' || *m_Current == ')')))
			break;

		lexeme += *m_Current++;
		m_Col++;
	}
	auto temp = lexeme;
	if(temp.empty())
		return Token();

	auto type = TokenType::Invalid;
	do {
		auto it = m_TokenTypes.find(lexeme);
		if (it != m_TokenTypes.end()) {
			type = it->second;
			break;
		}
		lexeme = lexeme.substr(0, lexeme.length() - 1);
	} while (!lexeme.empty());

	if (type == TokenType::Invalid)
		return Token();

	m_Current -= (temp.length() - lexeme.length());
	if (lexeme.empty()) {
		lexeme = temp;
		return Token{.Type = TokenType::Operator, .Lexeme = lexeme, .Line = m_Line, .Col = m_Col - (int)lexeme.length() };
	}
	return Token{.Type = type, .Lexeme = lexeme, .Line = m_Line, .Col = m_Col - (int)lexeme.length() };
}

Logo2::Token Logo2::Tokenizer::ParseString() {
	string lexeme;
	while (*++m_Current && *m_Current != '\"') {
		lexeme += *m_Current;
		m_Col++;
		if (*m_Current == '\n') {
			Token token{ .Type = TokenType::Error, .Lexeme = "Missing closing quote", .Line = m_Line, .Col = m_Col };
			m_Col = 1;
			m_Line++;
			return token;
		}
	}
	m_Current++;
	return Token { .Type = TokenType::String, .Lexeme = lexeme, .Line = m_Line, .Col = m_Col - (int)lexeme.length(), };
}
