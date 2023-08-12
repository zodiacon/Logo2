#pragma once

#include "Logo2Ast.h"

class Tokenizer;
struct Token;
enum class TokenType;
class Parser;

struct InfixParslet abstract {
	virtual std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) = 0;
	virtual int Precedence() const = 0;
};

struct PrefixParslet abstract {
	virtual std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) = 0;
	virtual int Precedence() const {
		return 0;
	}
};

enum class ParserError {
	NoError,
	UnknownOperator,
	IdentifierExpected,
	MissingInitExpression,
	SemicolonExpected,
	AssignExpected,
};

struct ParserException {
	ParserError Error;
	Token ErrorToken;
};

class Parser {
public:
	explicit Parser(Tokenizer& tokenizer);
	std::unique_ptr<LogoAstNode> Parse(std::string_view text, int line = 1);
	std::unique_ptr<LogoAstNode> ParseFile(std::string_view filename);

	bool AddParslet(TokenType type, std::unique_ptr<InfixParslet> parslet);
	bool AddParslet(TokenType type, std::unique_ptr<PrefixParslet> parslet);

	std::unique_ptr<Expression> ParseExpression(int precedence = 0);
	std::unique_ptr<VarStatement> ParseVarConstStatement(bool constant);

	Token Next();
	Token Peek() const;
	bool Match(TokenType type, bool consume = true);

private:
	void Init();
	std::unique_ptr<LogoAstNode> DoParse();
	int GetPrecedence() const;

	Tokenizer& m_Tokenizer;
	std::unordered_map<TokenType, std::unique_ptr<InfixParslet>> m_InfixParslets;
	std::unordered_map<TokenType, std::unique_ptr<PrefixParslet>> m_PrefixParslets;
	std::vector<Token> m_Tokens;
	size_t m_Current;
};

struct NumberParslet : PrefixParslet {
	std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
};

struct NameParslet : PrefixParslet {
	std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
};

struct GroupParslet : PrefixParslet {
	std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
	int Precedence() const override;
};

struct PrefixOperatorParslet : PrefixParslet {
	explicit PrefixOperatorParslet(int precedence);

	std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
	int Precedence() const override;

private:
	int m_Precedence;
};

struct PostfixOperatorParslet : InfixParslet {
	std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) override;
	int Precedence() const override;
};

struct BinaryOperatorParslet : InfixParslet {
	explicit BinaryOperatorParslet(int precedence, bool right = false);
	std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) override;
	int Precedence() const override;

private:
	int m_Precedence;
	bool m_RightAssoc;
};

struct AssignParslet : InfixParslet {
	std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) override;
	int Precedence() const override;
};
