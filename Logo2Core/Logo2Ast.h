#pragma once

#include "Token.h"
#include "Value.h"

namespace Logo2 {
	class Interpreter;

	enum class NodeType {
		Unknown = -1,
		Name,
	};

	class LogoAstNode abstract {
	public:
		virtual std::string ToString() const {
			return "";
		}

		virtual Value Accept(Interpreter* visitor) const = 0;
		virtual NodeType Type() const {
			return NodeType::Unknown;
		}
	};

	class Statement abstract : public LogoAstNode {
	};

	class Expression abstract : public LogoAstNode {
	};

	class ExpressionStatement final : public Statement {
	public:
		explicit ExpressionStatement(std::unique_ptr<Expression> expr);
		virtual Value Accept(Interpreter* visitor) const;
		Expression const* Expr() const;

	private:
		std::unique_ptr<Expression> m_Expr;
	};

	class AssignExpression : public Expression {
	public:
		AssignExpression(std::string name, std::unique_ptr<Expression> expr);
		Value Accept(Interpreter* visitor) const override;
		std::string const& Variable() const;
		Expression* const Value() const;

	private:
		std::string m_Name;
		std::unique_ptr<Expression> m_Expr;
	};

	class BlockExpression : public Expression {
	public:
		void Add(std::unique_ptr<LogoAstNode> node);
		Value Accept(Interpreter* visitor) const override;
		std::vector<LogoAstNode*> const Expressions() const;
		std::string ToString() const override;

	private:
		std::vector<std::unique_ptr<LogoAstNode>> m_Stmts;
	};

	class VarStatement : public Statement {
	public:
		VarStatement(std::string name, bool isConst, std::unique_ptr<Expression> init);
		Value Accept(Interpreter* visitor) const override;
		std::string ToString() const override;

		std::string const& Name() const;
		Expression const* Init() const;
		bool IsConst() const;

	private:
		std::string m_Name;
		std::unique_ptr<Expression> m_Init;
		bool m_IsConst;
	};

	class RepeatStatement : public Statement {
	public:
		RepeatStatement(std::unique_ptr<Expression> count, std::unique_ptr<BlockExpression> body);
		Value Accept(Interpreter* visitor) const override;

		Expression const* Count() const;
		BlockExpression const* Block() const;

	private:
		std::unique_ptr<Expression> m_Count;
		std::unique_ptr<BlockExpression> m_Block;
	};

	class WhileStatement : public Statement {
	public:
		WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<BlockExpression> body);
		Value Accept(Interpreter* visitor) const override;

		Expression const* Condition() const;
		BlockExpression const* Block() const;

	private:
		std::unique_ptr<Expression> m_Condition;
		std::unique_ptr<BlockExpression> m_Block;
	};

	class IfThenElseExpression : public Expression {
	public:
		IfThenElseExpression(std::unique_ptr<Expression> condition, std::unique_ptr<Expression> thenExpr, std::unique_ptr<Expression> elseExpr = nullptr);
		Value Accept(Interpreter* visitor) const override;

		Expression const* Condition() const;
		Expression const* Then() const;
		Expression const* Else() const;

	private:
		std::unique_ptr<Expression> m_Condition;
		std::unique_ptr<Expression> m_Then, m_Else;
	};

	class FunctionDeclaration : public Statement {
	public:
		FunctionDeclaration(std::string name, std::vector<std::string> parameters, std::unique_ptr<BlockExpression> body);
		Value Accept(Interpreter* visitor) const override;

		std::string const& Name() const;
		std::vector<std::string> const& Parameters() const;
		BlockExpression const* Body() const;
	private:
		std::string m_Name;
		std::vector<std::string> m_Parameters;
		std::unique_ptr<BlockExpression> m_Body;
	};

	class PostfixExpression : public Expression {
	public:
		PostfixExpression(std::unique_ptr<Expression> expr, Token token);
		Value Accept(Interpreter* visitor) const override;

		Token const& Operator() const;
		Expression const* Argument() const;

	private:
		Token m_Token;
		std::unique_ptr<Expression> m_Expr;
	};

	class BinaryExpression : public Expression {
	public:
		BinaryExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
		Value Accept(Interpreter* visitor) const override;

		std::string ToString() const override;

		Expression* Left() const;
		Expression* Right() const;
		Token const& Operator() const;

	private:
		std::unique_ptr<Expression> m_Left, m_Right;
		Token m_Operator;
	};

	class UnaryExpression : public Expression {
	public:
		UnaryExpression(Token op, std::unique_ptr<Expression> arg);
		Value Accept(Interpreter* visitor) const override;
		std::string ToString() const override;
		Token const& Operator() const;
		Expression* Arg() const;

	private:
		std::unique_ptr<Expression> m_Arg;
		Token m_Operator;
	};

	class LiteralExpression : public Expression {
	public:
		explicit LiteralExpression(Token token);
		Value Accept(Interpreter* visitor) const override;

		std::string ToString() const override;
		Token const& Literal() const;

	private:
		Token m_Token;
	};

	class NameExpression : public Expression {
	public:
		explicit NameExpression(std::string name);
		Value Accept(Interpreter* visitor) const override;
		std::string const& Name() const;
		NodeType Type() const override;
		std::string ToString() const override;

	private:
		std::string m_Name;
	};

	class InvokeFunctionExpression : public Expression {
	public:
		InvokeFunctionExpression(std::string name, std::vector<std::unique_ptr<Expression>> args);
		Value Accept(Interpreter* visitor) const override;
		std::string const& Name() const;
		std::vector<std::unique_ptr<Expression>> const& Arguments() const;

	private:
		std::string m_Name;
		std::vector<std::unique_ptr<Expression>> m_Arguments;
	};
}
