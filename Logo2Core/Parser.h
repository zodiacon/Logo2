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
		void AddError(ParserError err);
		bool HasErrors() const;
		std::span<const ParserError> Errors() const;

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

		Token Next();
		Token Peek() const;
		bool Match(TokenType type, bool consume = true);
		bool Match(std::string_view lexeme, bool consume = true);

		bool AddSymbol(Symbol sym);
		Symbol const* FindSymbol(std::string const& name) const;

	private:
		void Init();
		std::unique_ptr<LogoAstNode> DoParse();
		int GetPrecedence() const;

		Tokenizer& m_Tokenizer;
		std::unordered_map<TokenType, std::unique_ptr<InfixParslet>> m_InfixParslets;
		std::unordered_map<TokenType, std::unique_ptr<PrefixParslet>> m_PrefixParslets;
		std::vector<ParserError> m_Errors;
		std::vector<Token> m_Tokens;
		size_t m_Current;
		std::stack<std::unique_ptr<SymbolTable>> m_Symbols;
		int m_LoopCount{ 0 };
	};

}
