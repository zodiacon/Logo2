#include "pch.h"
#include "Parser.h"
#include "Logo2Ast.h"
#include <cassert>
#include "Tokenizer.h"
#include <fstream>

using namespace Logo2;
using namespace std;

Parser::Parser(Tokenizer& t) : m_Tokenizer(t) {
	Init();
	m_Symbols.push(make_unique<SymbolTable>());
}

unique_ptr<LogoAstNode> Parser::Parse(string text, int line) {
	m_Tokenizer.Tokenize(move(text), line);
	m_Errors.clear();
	return DoParse();
}

unique_ptr<LogoAstNode> Parser::ParseFile(string_view filename) {
	ifstream stm(filename.data());
	if (!stm.good())
		return nullptr;

	string text;
	char buf[2048];
	while (!stm.eof()) {
		stm.getline(buf, _countof(buf));
		text += buf;
		text += "\n";
	}
	return Parse(move(text));
}

bool Parser::AddParslet(TokenType type, unique_ptr<InfixParslet> parslet) {
	return m_InfixParslets.insert({ type, move(parslet) }).second;
}

bool Parser::AddParslet(TokenType type, unique_ptr<PrefixParslet> parslet) {
	return m_PrefixParslets.insert({ type, move(parslet) }).second;
}

void Parser::AddError(ParseError err) {
	m_Errors.emplace_back(move(err));
}

bool Parser::HasErrors() const {
	return !m_Errors.empty();
}

span<const ParseError> Parser::Errors() const {
	return m_Errors;
}

unique_ptr<Expression> Parser::ParseExpression(int precedence) {
	auto token = Next();
	if (auto it = m_PrefixParslets.find(token.Type); it != m_PrefixParslets.end()) {
		auto left = it->second->Parse(*this, token);
		while (precedence < GetPrecedence()) {
			auto token = Next();
			if (token.Type == TokenType::Invalid)
				break;
			if (auto it = m_InfixParslets.find(token.Type); it != m_InfixParslets.end())
				left = it->second->Parse(*this, move(left), token);
		}
		return left;
	}
	throw ParseError(ParseErrorType::UnknownOperator, token);
}

unique_ptr<VarStatement> Parser::ParseVarConstStatement(bool constant) {
	auto next = Next();		// eat var or const
	auto name = Next();		// variable name
	if (name.Type != TokenType::Identifier)
		throw ParseError(ParseErrorType::IdentifierExpected, name);

	{
		auto sym = FindSymbol(name.Lexeme, true);
		if (sym)
			throw ParseError(ParseErrorType::DuplicateDefinition, name, format("Symbol {} already defined in scope", name.Lexeme));
	}
	unique_ptr<Expression> init;
	if (Match(TokenType::Assign)) {
		//
		// init expression
		//
		init = ParseExpression();
	}
	else if (constant)
		throw ParseError(ParseErrorType::MissingInitExpression, Peek());

	if (!Match(TokenType::SemiColon))
		throw ParseError(ParseErrorType::SemicolonExpected, Peek());
	Symbol sym;
	sym.Name = name.Lexeme;
	sym.Type = SymbolType::Variable;
	sym.Flags = constant ? SymbolFlags::Const : SymbolFlags::None;
	if (!AddSymbol(sym))
		throw ParseError(ParseErrorType::DuplicateDefinition, name);
	return make_unique<VarStatement>(name.Lexeme, constant, move(init));
}

unique_ptr<FunctionDeclaration> Parser::ParseFunctionDeclaration() {
	Next();		// eat fn keyword
	auto ident = Next();
	if (ident.Type != TokenType::Identifier)
		throw ParseError(ParseErrorType::IdentifierExpected, ident);

	auto sym = FindSymbol(ident.Lexeme);
	if (sym)
		AddError(ParseError(ParseErrorType::DuplicateDefinition, ident));

	if (!Match(TokenType::OpenParen))
		throw ParseError(ParseErrorType::OpenParenExpected, ident);

	//
	// get list of arguments
	//
	vector<string> parameters;
	while (Peek().Type != TokenType::CloseParen) {
		auto param = Next();
		if (param.Type != TokenType::Identifier)
			throw ParseError(ParseErrorType::IdentifierExpected, ident);
		parameters.push_back(param.Lexeme);
		if (Match(TokenType::Comma))
			continue;
		if (!Match(TokenType::CloseParen, false))
			throw ParseError(ParseErrorType::CloseParenExpected, ident);
	}

	Next();		// eat close paren
	unique_ptr<Expression> body;
	if (Match(TokenType::GoesTo))
		body = ParseExpression();
	else
		body = ParseBlock(parameters);

	auto decl = make_unique<FunctionDeclaration>(move(ident.Lexeme), move(parameters), move(body));
	if (decl && sym == nullptr) {
		Symbol sym;
		sym.Name = decl->Name();
		sym.Type = SymbolType::Function;
		sym.Flags = SymbolFlags::None;
		AddSymbol(sym);
	}
	return decl;
}

unique_ptr<RepeatStatement> Parser::ParseRepeatStatement() {
	Next();		// eat "repeat"
	auto times = ParseExpression();

	m_LoopCount++;
	auto block = ParseBlock();
	m_LoopCount--;
	return make_unique<RepeatStatement>(move(times), move(block));
}

unique_ptr<WhileStatement> Parser::ParseWhileStatement() {
	Next();	// eat "while"
	auto cond = ParseExpression();
	if (cond == nullptr)
		AddError(ParseError(ParseErrorType::ConditionExpressionExpected, Peek()));
	m_LoopCount++;
	auto block = ParseBlock();
	m_LoopCount--;
	return make_unique<WhileStatement>(move(cond), move(block));
}

unique_ptr<BlockExpression> Parser::ParseBlock(vector<string> const& args) {
	if (!Match(TokenType::OpenBrace))
		AddError(ParseError(ParseErrorType::OpenBraceExpected, Peek()));

	m_Symbols.push(make_unique<SymbolTable>(m_Symbols.top().get()));

	for (auto& arg : args) {
		Symbol sym;
		sym.Name = arg;
		sym.Flags = SymbolFlags::None;
		sym.Type = SymbolType::Argument;
		AddSymbol(sym);
	}

	auto block = make_unique<BlockExpression>();
	while (Peek().Type != TokenType::CloseBrace) {
		auto stmt = ParseStatement();
		if (!stmt)
			break;
		block->Add(move(stmt));
	}
	Next();		// eat close brace
	m_Symbols.pop();
	return block;
}

unique_ptr<Statement> Parser::ParseStatement() {
	auto peek = Peek();
	if (peek.Type == TokenType::Invalid) {
		return nullptr;
	}

	switch (peek.Type) {
		case TokenType::Keyword_Var: return ParseVarConstStatement(false);
		case TokenType::Keyword_Const: return ParseVarConstStatement(true);
		case TokenType::Keyword_Repeat: return ParseRepeatStatement();
		case TokenType::Keyword_While: return ParseWhileStatement();
		case TokenType::Keyword_Fn: return ParseFunctionDeclaration();
		case TokenType::Keyword_Return: return ParseReturnStatement();
		case TokenType::Keyword_Break: return ParseBreakContinueStatement(false);
		case TokenType::Keyword_Continue: return ParseBreakContinueStatement(true);
		case TokenType::Keyword_For: return ParseForStatement();
		case TokenType::OpenBrace: return ParseBlock();
	}
	auto expr = ParseExpression();
	if (expr) {
		if (Match(TokenType::SemiColon))
			return make_unique<ExpressionStatement>(move(expr));
		return expr;
	}
	AddError(ParseError(ParseErrorType::InvalidStatement, peek));
	return nullptr;
}

unique_ptr<ReturnStatement> Parser::ParseReturnStatement() {
	Next();		// eat return keyword
	auto expr = ParseExpression();
	if (expr) {
		if (!Match(TokenType::SemiColon))
			AddError(ParseError(ParseErrorType::SemicolonExpected, Peek()));
		return make_unique<ReturnStatement>(move(expr));
	}
	return nullptr;
}

unique_ptr<BreakOrContinueStatement> Parser::ParseBreakContinueStatement(bool cont) {
	Next();		// eat keyword
	if (!Match(TokenType::SemiColon))
		AddError(ParseError(ParseErrorType::SemicolonExpected, Peek()));
	if(m_LoopCount == 0)
		AddError(ParseError(ParseErrorType::BreakContinueNoLoop, Peek()));

	return make_unique<BreakOrContinueStatement>(cont);
}

unique_ptr<ForStatement> Parser::ParseForStatement() {
	Next();		// eat for
	auto init = ParseStatement();
	if (!init) {
		AddError(ParseError(ParseErrorType::ExpressionOrVarExpected, Peek()));
	}
	auto whileExpr = ParseExpression();
	if (!Match(TokenType::SemiColon))
		AddError(ParseError(ParseErrorType::SemicolonExpected, Peek()));

	auto inc = ParseExpression();
	m_LoopCount++;
	auto body = ParseBlock();
	m_LoopCount--;
	return make_unique<ForStatement>(move(init), move(whileExpr), move(inc), move(body));
}

void Parser::Init() {
	vector<pair<string, TokenType>> tokens{
		{ "+", TokenType::Add },
		{ "-", TokenType::Sub },
		{ "*", TokenType::Mul },
		{ "/", TokenType::Div },
		{ "%", TokenType::Mod },
		{ "**", TokenType::Power },
		{ "&", TokenType::And },
		{ "|", TokenType::Or },
		{ "^", TokenType::Xor },

		{ "+=", TokenType::Assign_Add },
		{ "-=", TokenType::Assign_Sub },
		{ "*=", TokenType::Assign_Mul },
		{ "/=", TokenType::Assign_Div },
		{ "%=", TokenType::Assign_Mod },
		{ "**=", TokenType::Assign_Power },
		{ "&=", TokenType::Assign_And },
		{ "|=", TokenType::Assign_Or },
		{ "^=", TokenType::Assign_Xor },
		{ "==", TokenType::Equal },
		{ "!=", TokenType::NotEqual },
		{ "<", TokenType::LessThan },
		{ ">", TokenType::GreaterThan },
		{ "<=", TokenType::LessThanOrEqual },
		{ ">=", TokenType::GreaterThanOrEqual},

		{ "(", TokenType::OpenParen },
		{ ")", TokenType::CloseParen },
		{ "=", TokenType::Assign },
		{ "{", TokenType::OpenBrace },
		{ "}", TokenType::CloseBrace },
		{ "[", TokenType::OpenBracket },
		{ "]", TokenType::CloseBracket },
		{ ";", TokenType::SemiColon },
		{ ",", TokenType::Comma },
		{ "=>", TokenType::GoesTo },
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
		{ "for", TokenType::Keyword_For },
		{ "fn", TokenType::Keyword_Fn },
		{ "return", TokenType::Keyword_Return },
	};
	m_Tokenizer.AddTokens(tokens);

	AddParslet(TokenType::Add, make_unique<BinaryOperatorParslet>(100));
	AddParslet(TokenType::Sub, make_unique<BinaryOperatorParslet>(100));
	AddParslet(TokenType::Mul, make_unique<BinaryOperatorParslet>(200));
	AddParslet(TokenType::Div, make_unique<BinaryOperatorParslet>(200));
	AddParslet(TokenType::Mod, make_unique<BinaryOperatorParslet>(200));
	AddParslet(TokenType::Sub, make_unique<PrefixOperatorParslet>(300));
	AddParslet(TokenType::Integer, make_unique<NumberParslet>());
	AddParslet(TokenType::Keyword_True, make_unique<NumberParslet>());
	AddParslet(TokenType::Real, make_unique<NumberParslet>());
	AddParslet(TokenType::Identifier, make_unique<NameParslet>());
	AddParslet(TokenType::OpenParen, make_unique<GroupParslet>());
	AddParslet(TokenType::Power, make_unique<BinaryOperatorParslet>(30, true));
	AddParslet(TokenType::Assign, make_unique<AssignParslet>());
	AddParslet(TokenType::Equal, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::NotEqual, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::LessThan, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::LessThanOrEqual, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::GreaterThan, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::GreaterThanOrEqual, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::OpenParen, make_unique<InvokeFunctionParslet>());
	AddParslet(TokenType::Keyword_If, make_unique<IfThenElseParslet>());
	AddParslet(TokenType::And, make_unique<BinaryOperatorParslet>(400));
	AddParslet(TokenType::Or, make_unique<BinaryOperatorParslet>(390));
	AddParslet(TokenType::Xor, make_unique<BinaryOperatorParslet>(390));
	AddParslet(TokenType::Keyword_Fn, make_unique<AnonymousFunctionParslet>());
}

unique_ptr<Statements> Parser::DoParse() {
	auto block = make_unique<Statements>();
	while (true) {
		auto stmt = ParseStatement();
		if (stmt == nullptr)
			break;
		block->Add(move(stmt));
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

bool Parser::Match(string_view lexeme, bool consume) {
	auto next = Peek();
	if (consume && next.Lexeme == lexeme) {
		Next();
		return true;
	}
	return next.Lexeme == lexeme;
}

bool Parser::AddSymbol(Symbol sym) {
	return m_Symbols.top()->AddSymbol(move(sym));
}

Symbol const* Parser::FindSymbol(string const& name, bool localOnly) const {
	return m_Symbols.top()->FindSymbol(name, localOnly);
}


