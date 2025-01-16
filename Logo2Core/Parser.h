#pragma once

#include "Logo2Ast.h"
#include "Parslets.h"
#include <stack>
#include <span>
#include "SymbolTable.h"

namespace Logo2 {
	class SymbolTable;
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
		OpenParenExpected,
		CloseParenExpected,
		OpenBraceExpected,
		CloseBraceExpected,
		InvalidStatement,
		ConditionExpressionExpected,
		BreakContinueNoLoop,
		ExpressionOrVarExpected,
		IllegalExpression,
	};

	struct ParseError {
		explicit ParseError(ParseErrorType type) : Error(type) {}
		ParseError(ParseErrorType type, Token token, std::string text = "") : Error(type), ErrorToken(std::move(token)), ErrorText(std::move(text)) {}

		ParseErrorType Error;
		Token ErrorToken;
		std::string ErrorText;	// optional
	};

	class Parser {
	public:
		explicit Parser(Tokenizer& tokenizer);
		std::unique_ptr<LogoAstNode> Parse(std::string text, int line = 1);
		std::unique_ptr<LogoAstNode> ParseFile(std::string_view filename);

		bool AddParslet(TokenType type, std::unique_ptr<InfixParslet> parslet);
		bool AddParslet(TokenType type, std::unique_ptr<PrefixParslet> parslet);
		void AddError(ParseError err);
		bool HasErrors() const;
		std::span<const ParseError> Errors() const;

		std::unique_ptr<Expression> ParseExpression(int precedence = 0);
		std::unique_ptr<VarStatement> ParseVarConstStatement(bool constant);
		std::unique_ptr<FunctionDeclaration> ParseFunctionDeclaration();
		std::unique_ptr<RepeatStatement> ParseRepeatStatement();
		std::unique_ptr<WhileStatement> ParseWhileStatement();
		std::unique_ptr<BlockExpression> ParseBlock(std::vector<std::string> const& args = {});
		std::unique_ptr<Statement> ParseStatement();
		std::unique_ptr<ReturnStatement> ParseReturnStatement();
		std::unique_ptr<BreakOrContinueStatement> ParseBreakContinueStatement(bool cont);
		std::unique_ptr<ForStatement> ParseForStatement();
		std::unique_ptr<EnumDeclaration> ParseEnumDeclaration();

		Token Next();
		Token Peek() const;
		bool SkipTo(TokenType type);
		bool Match(TokenType type, bool consume = true, bool errorIfNotFound = false);
		bool Match(std::string_view lexeme, bool consume = true, bool errorIfNotFound = false);

		bool AddSymbol(Symbol sym);
		Symbol const* FindSymbol(std::string const& name, bool localOnly = false) const;

	private:
		void PushScope();
		void PopScope();
		void Init();
		std::unique_ptr<Statements> DoParse();
		int GetPrecedence() const;

		Tokenizer& m_Tokenizer;
		std::unordered_map<TokenType, std::unique_ptr<InfixParslet>> m_InfixParslets;
		std::unordered_map<TokenType, std::unique_ptr<PrefixParslet>> m_PrefixParslets;
		std::vector<ParseError> m_Errors;
		std::vector<Token> m_Tokens;
		size_t m_Current;
		std::stack<std::unique_ptr<SymbolTable>> m_Symbols;
		std::stack<std::string> m_Namespaces;
		int m_LoopCount{ 0 };
	};

}
