#include "pch.h"
#include "Parslets.h"
#include "Parser.h"

using namespace Logo2;
using namespace std;

PostfixOperatorParslet::PostfixOperatorParslet(int precedence) : m_Precedence(precedence) {
}

unique_ptr<Expression> PostfixOperatorParslet::Parse(Parser& parser, unique_ptr<Expression> left, Token const& token) {
	return make_unique<PostfixExpression>(move(left), token);
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

unique_ptr<Expression> NameParslet::Parse(Parser& parser, Token const& token) {
	auto name = token.Lexeme;
	while (parser.Peek().Type == TokenType::ScopeRes) {
		parser.Next();
		if (parser.Peek().Type != TokenType::Identifier) {
			parser.AddError(ParseError(ParseErrorType::IdentifierExpected, parser.Peek(), "Identifier expected after ::"));
			break;
		}
		name += "::" + parser.Next().Lexeme;
	}
	return make_unique<NameExpression>(name);
}

unique_ptr<Expression> PrefixOperatorParslet::Parse(Parser& parser, Token const& token) {
	return make_unique<UnaryExpression>(token, parser.ParseExpression(m_Precedence));
}

unique_ptr<Expression> LiteralParslet::Parse(Parser& parser, Token const& token) {
	return make_unique<LiteralExpression>(token);
}

unique_ptr<Expression> BinaryOperatorParslet::Parse(Parser& parser, unique_ptr<Expression> left, Token const& token) {
	auto right = parser.ParseExpression(m_Precedence - (m_RightAssoc ? 1 : 0));
	return make_unique<BinaryExpression>(move(left), token, move(right));
}

int GroupParslet::Precedence() const {
	return 1000;
}

unique_ptr<Expression> GroupParslet::Parse(Parser& parser, Token const& token) {
	auto expr = parser.ParseExpression();
	parser.Match(TokenType::CloseParen);
	return expr;
}

unique_ptr<Expression> AssignParslet::Parse(Parser& parser, unique_ptr<Expression> left, Token const& token) {
	auto right = parser.ParseExpression(Precedence() - 1);
	if (left->Type() != NodeType::Name) {
		throw ParseError(ParseErrorType::IdentifierExpected, token);
	}
	auto nameExpr = reinterpret_cast<NameExpression*>(left.get());
	auto sym = parser.FindSymbol(nameExpr->Name());
	if (!sym)
		throw ParseError(ParseErrorType::UndefinedSymbol, token);

	if ((sym->Flags & SymbolFlags::Const) == SymbolFlags::Const)
		throw ParseError(ParseErrorType::CannotModifyConst, token);
	parser.Match(TokenType::SemiColon);
	return make_unique<AssignExpression>(nameExpr->Name(), move(right));
}

int AssignParslet::Precedence() const {
	return 2;
}

InvokeFunctionParslet::InvokeFunctionParslet() : PostfixOperatorParslet(1200) {
}

unique_ptr<Expression> InvokeFunctionParslet::Parse(Parser& parser, unique_ptr<Expression> left, Token const& token) {
	if (left->Type() != NodeType::Name)
		throw ParseError(ParseErrorType::Syntax, token);

	auto nameExpr = reinterpret_cast<NameExpression*>(left.get());
	//auto sym = parser.FindSymbol(nameExpr->Name());
	//if (sym == nullptr)
	//	parser.AddError(ParserError(ParseErrorType::UndefinedSymbol, token));

	auto next = parser.Peek();
	vector<unique_ptr<Expression>> args;
	while (next.Type != TokenType::CloseParen) {
		auto param = parser.ParseExpression();
		args.push_back(move(param));
		if (!parser.Match(TokenType::Comma) && !parser.Match(TokenType::CloseParen, false))
			throw ParseError(ParseErrorType::CommaExpected, next);
		next = parser.Peek();
	}
	parser.Next();		// eat close paren
	return make_unique<InvokeFunctionExpression>(nameExpr->Name(), move(args));
}

unique_ptr<Expression> Logo2::IfThenElseParslet::Parse(Parser& parser, Token const& token) {
	auto cond = parser.ParseExpression();
	auto then = parser.ParseBlock();
	unique_ptr<Expression> elseExpr;
	if (parser.Match(TokenType::Keyword_Else))
		elseExpr = parser.ParseBlock();
	return make_unique<IfThenElseExpression>(move(cond), move(then), move(elseExpr));
}

unique_ptr<Expression> Logo2::AnonymousFunctionParslet::Parse(Parser& parser, Token const& token) {
	assert(token.Type == TokenType::Keyword_Fn);
	if (!parser.Match(TokenType::OpenParen))
		throw ParseError(ParseErrorType::OpenParenExpected, parser.Peek());

	//
	// parse args
	//
	vector<string> args;
	while (parser.Peek().Type != TokenType::CloseParen) {
		auto arg = parser.Next();
		if(arg.Type != TokenType::Identifier)
			throw ParseError(ParseErrorType::IdentifierExpected, arg);
		args.push_back(move(arg.Lexeme));
		if (parser.Match(TokenType::Comma) || parser.Match(TokenType::CloseParen, false))
			continue;
		throw ParseError(ParseErrorType::CommaOrCloseParenExpected, parser.Peek());
	}
	parser.Next();		// eat close paren
	if (parser.Match(TokenType::GoesTo)) {
		auto expr = parser.ParseExpression();
		return make_unique<AnonymousFunctionExpression>(move(args), move(expr));
	}
	auto block = parser.ParseBlock(args);
	return make_unique<AnonymousFunctionExpression>(move(args), move(block));
}

int Logo2::AnonymousFunctionParslet::Precedence() const {
	return 2000;
}
