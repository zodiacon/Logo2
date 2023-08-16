#pragma once

namespace Logo2 {
	enum class TokenType {
		Invalid,
		Error,
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
		Assign_Add,
		Assign_Sub,
		Assign_Mul,
		Assign_Div,
		Assign_Mod,
		Assign_Neg,
		Assign_Power,
		Assign_And,
		Assign_Or,
		Assign_Xor,

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
		Keyword_Break,
		Keyword_Continue,
	};

	struct Token {
		TokenType Type;
		std::string Lexeme;
		int Line, Col;
	};
}
