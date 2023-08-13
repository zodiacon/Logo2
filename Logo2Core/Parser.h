#pragma once

#include "Logo2Ast.h"
#include "Parslets.h"
#include <stack>
#include "SymbolTable.h"

class Tokenizer;
struct Token;
enum class TokenType;
class Parser;

enum class ParseErrorType {
	NoError,
	Syntax,
	UnknownOperator,
	IdentifierExpected,
	MissingInitExpression,
	SemicolonExpected,
	AssignExpected,
	CommaExpected,
	CommaOrCloseParenExpected,
	DuplicateDefinition,
	UndefinedSymbol,
	CannotModifyConst,
};

struct ParserError {
	ParseErrorType Error;
	Token ErrorToken;
};

class Parser {
public:
	explicit Parser(Tokenizer& tokenizer);
	std::unique_ptr<LogoAstNode> Parse(std::string text, int line = 1);
	std::unique_ptr<LogoAstNode> ParseFile(std::string_view filename);

	bool AddParslet(TokenType type, std::unique_ptr<InfixParslet> parslet);
	bool AddParslet(TokenType type, std::unique_ptr<PrefixParslet> parslet);

	std::unique_ptr<Expression> ParseExpression(int precedence = 0);
	std::unique_ptr<VarStatement> ParseVarConstStatement(bool constant);

	Token Next();
	Token Peek() const;
	bool Match(TokenType type, bool consume = true);

	bool AddSymbol(Symbol sym);
	Symbol const* FindSymbol(std::string const& name) const;

private:
	void Init();
	std::unique_ptr<LogoAstNode> DoParse();
	int GetPrecedence() const;

	Tokenizer& m_Tokenizer;
	std::unordered_map<TokenType, std::unique_ptr<InfixParslet>> m_InfixParslets;
	std::unordered_map<TokenType, std::unique_ptr<PrefixParslet>> m_PrefixParslets;
	std::vector<Token> m_Tokens;
	size_t m_Current;
	std::stack<SymbolTable> m_Symbols;
	bool m_HasErrors{ false };
};

