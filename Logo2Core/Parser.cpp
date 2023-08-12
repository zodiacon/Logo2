#include "pch.h"
#include "Parser.h"
#include "Logo2Ast.h"
#include <cassert>
#include "Tokenizer.h"

Parser::Parser(Tokenizer& t) : m_Tokenizer(t) {
	Init();
}

std::unique_ptr<LogoAstNode> Parser::Parse(std::string_view text, int line) {
	m_Tokenizer.Tokenize(text, line);
	return DoParse();
}

bool Parser::AddParslet(TokenType type, std::unique_ptr<InfixParslet> parslet) {
	return m_InfixParslets.insert({ type, std::move(parslet) }).second;
}

bool Parser::AddParslet(TokenType type, std::unique_ptr<PrefixParslet> parslet) {
	return m_PrefixParslets.insert({ type, std::move(parslet) }).second;
}

std::unique_ptr<Expression> NameParslet::Parse(Parser& parser, Token const& token) {
	return std::make_unique<NameExpression>(token.Lexeme);
}

std::unique_ptr<Expression> PrefixOperatorParslet::Parse(Parser& parser, Token const& token) {
	return std::make_unique<UnaryExpression>(token, parser.ParseExpression(m_Precedence));
}

std::unique_ptr<Expression> NumberParslet::Parse(Parser& parser, Token const& token) {
	return std::make_unique<LiteralExpression>(token);
}

std::unique_ptr<Expression> BinaryOperatorParslet::Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) {
	auto right = parser.ParseExpression(m_Precedence - (m_RightAssoc ? 1 : 0));
	return std::make_unique<BinaryExpression>(std::move(left), token, std::move(right));
}

std::unique_ptr<Expression> PostfixOperatorParslet::Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) {
	return std::make_unique<PostfixExpression>(std::move(left), token);
}

BinaryOperatorParslet::BinaryOperatorParslet(int precedence, bool right) : m_Precedence(precedence), m_RightAssoc(right) {
}

PrefixOperatorParslet::PrefixOperatorParslet(int precedence) : m_Precedence(precedence) {
}

int BinaryOperatorParslet::Precedence() const {
	return m_Precedence;
}

int PrefixOperatorParslet::Precedence() const {
	return m_Precedence;
}

int PostfixOperatorParslet::Precedence() const {
	return 0;
}

std::unique_ptr<Expression> Parser::ParseExpression(int precedence) {
	auto token = Next();
	if (auto it = m_PrefixParslets.find(token.Type); it != m_PrefixParslets.end()) {
		auto left = it->second->Parse(*this, token);
		while (precedence < GetPrecedence()) {
			auto token = Next();
			if (token.Type == TokenType::Invalid)
				break;
			if (auto it = m_InfixParslets.find(token.Type); it != m_InfixParslets.end())
				left = it->second->Parse(*this, std::move(left), token);
		}
		return left;
	}
	throw ParserException(ParserError::UnknownOperator, token);
}

std::unique_ptr<VarStatement> Parser::ParseVarConstStatement(bool constant) {
	auto next = Next();		// var or const
	auto name = Next();
	if (name.Type != TokenType::Identifier)
		throw ParserException(ParserError::IdentifierExpected, name);

	std::unique_ptr<Expression> init;
	if (Match(TokenType::Assign)) {
		//
		// init expression
		//
		init = ParseExpression();
	}
	else if (constant)
		throw ParserException(ParserError::MissingInitExpression, Peek());
	else
		throw ParserException(ParserError::AssignExpected, Peek());

	if (!Match(TokenType::SemiColon))
		throw ParserException(ParserError::SemicolonExpected, Peek());
	return std::make_unique<VarStatement>(name.Lexeme, constant, std::move(init));
}

void Parser::Init() {
	std::vector<std::pair<std::string, TokenType>> tokens{
		{ "+", TokenType::Add },
		{ "-", TokenType::Sub },
		{ "*", TokenType::Mul },
		{ "/", TokenType::Div },
		{ "%", TokenType::Mod },
		{ "if", TokenType::Keyword_If },
		{ "**", TokenType::Power },
		{ "(", TokenType::OpenParen },
		{ ")", TokenType::CloseParen },
		{ "&", TokenType::And },
		{ "|", TokenType::Or },
		{ "^", TokenType::Xor },
		{ "=", TokenType::Assign },
		{ "{", TokenType::OpenBrace },
		{ "}", TokenType::CloseBrace },
		{ "[", TokenType::OpenBracket },
		{ "]", TokenType::CloseBracket },
		{ ";", TokenType::SemiColon },
		{ "null", TokenType::Keyword_Null },
		{ "true", TokenType::Keyword_True },
		{ "false", TokenType::Keyword_False },
		{ "var", TokenType::Keyword_Var },
		{ "const", TokenType::Keyword_Const },
		{ "if", TokenType::Keyword_If },
		{ "repeat", TokenType::Keyword_Repeat },
		{ "else", TokenType::Keyword_Else },
		{ "==", TokenType::Equal },
		{ "!=", TokenType::NotEqual },
		{ "<", TokenType::LessThan },
		{ ">", TokenType::GreaterThan },
		{ "<=", TokenType::LessThanOrEqual },
		{ ">=", TokenType::GreaterThanOrEqual},
	};
	m_Tokenizer.AddTokens(tokens);

	AddParslet(TokenType::Add, std::make_unique<BinaryOperatorParslet>(100));
	AddParslet(TokenType::Sub, std::make_unique<BinaryOperatorParslet>(100));
	AddParslet(TokenType::Mul, std::make_unique<BinaryOperatorParslet>(200));
	AddParslet(TokenType::Div, std::make_unique<BinaryOperatorParslet>(200));
	AddParslet(TokenType::Mod, std::make_unique<BinaryOperatorParslet>(200));
	AddParslet(TokenType::Sub, std::make_unique<PrefixOperatorParslet>(300));
	AddParslet(TokenType::Integer, std::make_unique<NumberParslet>());
	AddParslet(TokenType::Keyword_True, std::make_unique<NumberParslet>());
	AddParslet(TokenType::Real, std::make_unique<NumberParslet>());
	AddParslet(TokenType::Identifier, std::make_unique<NameParslet>());
	AddParslet(TokenType::OpenParen, std::make_unique<GroupParslet>());
	AddParslet(TokenType::Power, std::make_unique<BinaryOperatorParslet>(30, true));
	AddParslet(TokenType::Assign, std::make_unique<AssignParslet>());
	AddParslet(TokenType::Equal, std::make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::NotEqual, std::make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::LessThan, std::make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::LessThanOrEqual, std::make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::GreaterThan, std::make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::GreaterThanOrEqual, std::make_unique<BinaryOperatorParslet>(90));

}

std::unique_ptr<LogoAstNode> Parser::DoParse() {
	auto block = std::make_unique<BlockExpression>();
	while (true) {
		auto peek = Peek();
		if (peek.Type == TokenType::Invalid)
			break;
		switch (peek.Type) {
			case TokenType::Keyword_Var: block->Add(ParseVarConstStatement(false)); break;
			case TokenType::Keyword_Const: block->Add(ParseVarConstStatement(true)); break;
			default: block->Add(ParseExpression()); break;
		}
	}
	return block;
}

int Parser::GetPrecedence() const {
	auto token = Peek();
	if (auto it = m_InfixParslets.find(token.Type); it != m_InfixParslets.end())
		return it->second->Precedence();
	return 0;
}

Token Parser::Next() {
	return m_Tokenizer.Next();
}

Token Parser::Peek() const {
	return m_Tokenizer.Peek();
}

bool Parser::Match(TokenType type, bool consume) {
	auto next = Peek();
	if (consume && next.Type == type) {
		Next();
		return true;
	}
	return next.Type == type;
}

std::unique_ptr<Expression> GroupParslet::Parse(Parser& parser, Token const& token) {
	auto expr = parser.ParseExpression();
	parser.Match(TokenType::CloseParen);
	return expr;
}

int GroupParslet::Precedence() const {
	return 1000;
}

std::unique_ptr<Expression> AssignParslet::Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) {
	auto right = parser.ParseExpression(Precedence() - 1);
	if (left->Type() != NodeType::Name) {
		throw ParserException(ParserError::IdentifierExpected, token);
	}
	auto nameExpr = reinterpret_cast<NameExpression*>(left.get());
	parser.Match(TokenType::SemiColon);
	return std::make_unique<AssignExpression>(nameExpr->Name(), std::move(right));
}

int AssignParslet::Precedence() const {
	return 2;
}
