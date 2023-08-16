#include "pch.h"
#include "Parslets.h"
#include "Parser.h"

using namespace Logo2;

PostfixOperatorParslet::PostfixOperatorParslet(int precedence) : m_Precedence(precedence) {
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
	return m_Precedence;
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

int GroupParslet::Precedence() const {
	return 1000;
}

std::unique_ptr<Expression> GroupParslet::Parse(Parser& parser, Token const& token) {
	auto expr = parser.ParseExpression();
	parser.Match(TokenType::CloseParen);
	return expr;
}

std::unique_ptr<Expression> AssignParslet::Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) {
	auto right = parser.ParseExpression(Precedence() - 1);
	if (left->Type() != NodeType::Name) {
		throw ParserError(ParseErrorType::IdentifierExpected, token);
	}
	auto nameExpr = reinterpret_cast<NameExpression*>(left.get());
	auto sym = parser.FindSymbol(nameExpr->Name());
	if (!sym)
		throw ParserError(ParseErrorType::UndefinedSymbol, token);

	if ((sym->Flags & SymbolFlags::Const) == SymbolFlags::Const)
		throw ParserError(ParseErrorType::CannotModifyConst, token);
	parser.Match(TokenType::SemiColon);
	return std::make_unique<AssignExpression>(nameExpr->Name(), std::move(right));
}

int AssignParslet::Precedence() const {
	return 2;
}

InvokeFunctionParslet::InvokeFunctionParslet() : PostfixOperatorParslet(1200) {
}

std::unique_ptr<Expression> InvokeFunctionParslet::Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) {
	if (left->Type() != NodeType::Name)
		throw ParserError(ParseErrorType::Syntax, token);

	auto nameExpr = reinterpret_cast<NameExpression*>(left.get());
	//auto sym = parser.FindSymbol(nameExpr->Name());
	//if (sym == nullptr)
	//	parser.AddError(ParserError(ParseErrorType::UndefinedSymbol, token));

	auto next = parser.Peek();
	std::vector<std::unique_ptr<Expression>> args;
	while (next.Type != TokenType::CloseParen) {
		auto param = parser.ParseExpression();
		args.push_back(std::move(param));
		if (!parser.Match(TokenType::Comma) && !parser.Match(TokenType::CloseParen, false))
			throw ParserError(ParseErrorType::CommaExpected, next);
		next = parser.Peek();
	}
	parser.Next();		// eat close paren
	return std::make_unique<InvokeFunctionExpression>(nameExpr->Name(), std::move(args));
}

std::unique_ptr<Expression> Logo2::IfThenElseParslet::Parse(Parser& parser, Token const& token) {
	auto cond = parser.ParseExpression();
	auto then = parser.ParseBlock();
	std::unique_ptr<Expression> elseExpr;
	if (parser.Match(TokenType::Keyword_Else))
		elseExpr = parser.ParseBlock();
	return std::make_unique<IfThenElseExpression>(std::move(cond), std::move(then), std::move(elseExpr));
}
