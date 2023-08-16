#include "pch.h"
#include "Parser.h"
#include "Logo2Ast.h"
#include <cassert>
#include "Tokenizer.h"
#include <fstream>

Logo2::Parser::Parser(Tokenizer& t) : m_Tokenizer(t) {
	Init();
	m_Symbols.push(std::make_unique<SymbolTable>());
}

std::unique_ptr<Logo2::LogoAstNode> Logo2::Parser::Parse(std::string text, int line) {
	m_Tokenizer.Tokenize(std::move(text), line);
	m_Errors.clear();
	return DoParse();
}

std::unique_ptr<Logo2::LogoAstNode> Logo2::Parser::ParseFile(std::string_view filename) {
	std::ifstream stm(filename.data());
	if (!stm.good())
		return nullptr;

	std::string text;
	char buf[2048];
	while (!stm.eof()) {
		stm.getline(buf, _countof(buf));
		text += buf;
		text += "\n";
	}
	return Parse(std::move(text));
}

bool Logo2::Parser::AddParslet(TokenType type, std::unique_ptr<InfixParslet> parslet) {
	return m_InfixParslets.insert({ type, std::move(parslet) }).second;
}

bool Logo2::Parser::AddParslet(TokenType type, std::unique_ptr<PrefixParslet> parslet) {
	return m_PrefixParslets.insert({ type, std::move(parslet) }).second;
}

void Logo2::Parser::AddError(ParserError err) {
	m_Errors.emplace_back(std::move(err));
}

bool Logo2::Parser::HasErrors() const {
	return !m_Errors.empty();
}

std::span<const Logo2::ParserError> Logo2::Parser::Errors() const {
	return m_Errors;
}

std::unique_ptr<Logo2::Expression> Logo2::Parser::ParseExpression(int precedence) {
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
	throw ParserError(ParseErrorType::UnknownOperator, token);
}

std::unique_ptr<Logo2::VarStatement> Logo2::Parser::ParseVarConstStatement(bool constant) {
	auto next = Next();		// var or const
	auto name = Next();
	if (name.Type != TokenType::Identifier)
		throw ParserError(ParseErrorType::IdentifierExpected, name);

	std::unique_ptr<Expression> init;
	if (Match(TokenType::Assign)) {
		//
		// init expression
		//
		init = ParseExpression();
	}
	else if (constant)
		throw ParserError(ParseErrorType::MissingInitExpression, Peek());
	else
		throw ParserError(ParseErrorType::AssignExpected, Peek());

	if (!Match(TokenType::SemiColon))
		throw ParserError(ParseErrorType::SemicolonExpected, Peek());
	Symbol sym;
	sym.Name = name.Lexeme;
	sym.Type = SymbolType::Variable;
	sym.Flags = constant ? SymbolFlags::Const : SymbolFlags::None;
	if (!AddSymbol(sym))
		throw ParserError(ParseErrorType::DuplicateDefinition, name);
	return std::make_unique<VarStatement>(name.Lexeme, constant, std::move(init));
}

std::unique_ptr<Logo2::RepeatStatement> Logo2::Parser::ParseRepeatStatement() {
	Next();		// eat "repeat"
	auto times = ParseExpression();

	auto block = ParseBlock();
	return std::make_unique<RepeatStatement>(std::move(times), std::move(block));
}

std::unique_ptr<Logo2::WhileStatement> Logo2::Parser::ParseWhileStatement() {
	Next();	// eat "while"
	auto cond = ParseExpression();
	if (cond == nullptr)
		AddError(ParserError(ParseErrorType::ConditionExpressionExpected, Peek()));
	return std::make_unique<WhileStatement>(std::move(cond), ParseBlock());
}

std::unique_ptr<Logo2::BlockExpression> Logo2::Parser::ParseBlock() {
	if (!Match(TokenType::OpenBrace))
		AddError(ParserError(ParseErrorType::OpenBraceExpected, Peek()));

	m_Symbols.push(std::make_unique<SymbolTable>(m_Symbols.top().get()));
	auto block = std::make_unique<BlockExpression>();
	while (Peek().Type != TokenType::CloseBrace) {
		auto stmt = ParseStatement();
		if (!stmt)
			break;
		block->Add(std::move(stmt));
	}
	Next();		// eat close brace
	m_Symbols.pop();
	return block;
}

std::unique_ptr<Logo2::Statement> Logo2::Parser::ParseStatement() {
	auto peek = Peek();
	if (peek.Type == TokenType::Invalid) {
		return nullptr;
	}

	switch (peek.Type) {
		case TokenType::Keyword_Var: return ParseVarConstStatement(false);
		case TokenType::Keyword_Const: return ParseVarConstStatement(true);
		case TokenType::Keyword_Repeat: return ParseRepeatStatement();
		case TokenType::Keyword_While: return ParseWhileStatement();
		//case TokenType::Keyword_Break: return ParseBreakStatement();
	}
	auto expr = ParseExpression();
	if (expr) {
		Match(TokenType::SemiColon);
		return std::make_unique<ExpressionStatement>(std::move(expr));
	}
	AddError(ParserError(ParseErrorType::InvalidStatement, peek));
	return nullptr;
}

void Logo2::Parser::Init() {
	std::vector<std::pair<std::string, TokenType>> tokens{
		{ "+", TokenType::Add },
		{ "-", TokenType::Sub },
		{ "*", TokenType::Mul },
		{ "/", TokenType::Div },
		{ "%", TokenType::Mod },
		{ "**", TokenType::Power },

		{ "+=", TokenType::Assign_Add },
		{ "-=", TokenType::Assign_Sub },
		{ "*=", TokenType::Assign_Mul },
		{ "/=", TokenType::Assign_Div },
		{ "%=", TokenType::Assign_Mod },
		{ "**=", TokenType::Assign_Power },
		{ "&=", TokenType::Assign_And },
		{ "|=", TokenType::Assign_Or },
		{ "^=", TokenType::Assign_Xor },

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
		{ ",", TokenType::Comma },
		{ "null", TokenType::Keyword_Null },
		{ "true", TokenType::Keyword_True },
		{ "false", TokenType::Keyword_False },
		{ "var", TokenType::Keyword_Var },
		{ "const", TokenType::Keyword_Const },
		{ "if", TokenType::Keyword_If },
		{ "repeat", TokenType::Keyword_Repeat },
		{ "while", TokenType::Keyword_While },
		{ "break", TokenType::Keyword_Break },
		{ "continue", TokenType::Keyword_Continue },
		{ "else", TokenType::Keyword_Else },
		{ "fn", TokenType::Keyword_Fn },
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
	AddParslet(TokenType::OpenParen, std::make_unique<InvokeFunctionParslet>());
	AddParslet(TokenType::Keyword_If, std::make_unique<IfThenElseParslet>());
}

std::unique_ptr<Logo2::LogoAstNode> Logo2::Parser::DoParse() {
	auto block = std::make_unique<BlockExpression>();
	while (true) {
		auto stmt = ParseStatement();
		if (stmt == nullptr)
			break;
		block->Add(std::move(stmt));
	}
	return block;
}

int Logo2::Parser::GetPrecedence() const {
	auto token = Peek();
	if (auto it = m_InfixParslets.find(token.Type); it != m_InfixParslets.end())
		return it->second->Precedence();
	return 0;
}

Logo2::Token Logo2::Parser::Next() {
	return m_Tokenizer.Next();
}

Logo2::Token Logo2::Parser::Peek() const {
	return m_Tokenizer.Peek();
}

bool Logo2::Parser::Match(TokenType type, bool consume) {
	auto next = Peek();
	if (consume && next.Type == type) {
		Next();
		return true;
	}
	return next.Type == type;
}

bool Logo2::Parser::AddSymbol(Logo2::Symbol sym) {
	return m_Symbols.top()->AddSymbol(std::move(sym));
}

Logo2::Symbol const* Logo2::Parser::FindSymbol(std::string const& name) const {
	return m_Symbols.top()->FindSymbol(name);
}


