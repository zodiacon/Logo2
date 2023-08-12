#pragma once

enum class TokenType {
	Invalid,
	Integer,
	Real,
	String,
	Identifier,

	Add = 0x20,
	Sub,
	Mul,
	Div,
	Mod,
	Neg,
	Power,
	And,
	Or,
	Xor,
	Not,
	OpenParen,
	CloseParen,
	OpenBrace,
	CloseBrace,
	OpenBracket,
	CloseBracket,
	Dot,
	Comma,
	SemiColon,
	Assign,
	Equal,
	NotEqual,
	GreaterThan,
	LessThan,
	GreaterThanOrEqual,
	LessThanOrEqual,

	Operator = 0x5f,

	Keyword = 0x60,
	Keyword_Repeat,
	Keyword_If,
	Keyword_Else,
	Keyword_Fn,
	Keyword_Return,
	Keyword_While,
	Keyword_Null,
	Keyword_Var,
	Keyword_Const,
	Keyword_True,
	Keyword_False,
	Keyword_For,
	Keyword_Do,
};

struct Token {
	TokenType Type;
	std::string Lexeme;
	int Line, Col;
};

