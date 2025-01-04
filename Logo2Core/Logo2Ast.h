#pragma once

#include "Token.h"
#include "Value.h"
#include "Visitor.h"

namespace Logo2 {
	enum class NodeType {
		Invalid,
		Name,
		For,
		Var,
	};

	class LogoAstNode abstract {
	public:
		virtual ~LogoAstNode() = default;
		virtual std::string ToString() const {
			return "";
		}

		virtual NodeType Type() const {
			return NodeType::Invalid;
		}
		virtual Value Accept(Visitor* visitor) const = 0;
		virtual bool IsStatement() const {
			return false;
		}
		
		virtual bool IsExpression() const {
			return false;
		}
	};

	class Statement abstract : public LogoAstNode {
	public:
		bool IsStatement() const override;
	};

	class Statements final : public Statement {
	public:
		Value Accept(Visitor* visitor) const override;
		void Add(std::unique_ptr<Statement> stmt);
		std::vector<std::unique_ptr<Statement>> const& Get() const;

	private:
		std::vector<std::unique_ptr<Statement>> m_Stmts;
	};

	class Expression abstract : public Statement {
	public:
		virtual bool IsExpression() const override {
			return true;
		}
	};

	class ExpressionStatement final : public Statement {
	public:
		explicit ExpressionStatement(std::unique_ptr<Expression> expr);
		Value Accept(Visitor* visitor) const override;
		Expression const* Expr() const;

	private:
		std::unique_ptr<Expression> m_Expr;
	};

	class AssignExpression : public Expression {
	public:
		AssignExpression(std::string name, std::unique_ptr<Expression> expr);
		Value Accept(Visitor* visitor) const override;
		std::string const& Variable() const;
		Expression* const Value() const;

	private:
		std::string m_Name;
		std::unique_ptr<Expression> m_Expr;
	};

	class BlockExpression : public Expression {
	public:
		void Add(std::unique_ptr<LogoAstNode> node);
		Value Accept(Visitor* visitor) const override;
		std::vector<LogoAstNode*> const Expressions() const;
		std::string ToString() const override;

	private:
		std::vector<std::unique_ptr<LogoAstNode>> m_Stmts;
	};

	class VarStatement : public Statement {
	public:
		VarStatement(std::string name, bool isConst, std::unique_ptr<Expression> init);
		NodeType Type() const override {
			return NodeType::Var;
		}
		Value Accept(Visitor* visitor) const override;
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
		Value Accept(Visitor* visitor) const override;

		Expression const* Count() const;
		BlockExpression const* Block() const;

	private:
		std::unique_ptr<Expression> m_Count;
		std::unique_ptr<BlockExpression> m_Block;
	};

	class BreakOrContinueStatement : public Statement {
	public:
		explicit BreakOrContinueStatement(bool cont);
		Value Accept(Visitor* visitor) const override;
		bool IsContinue() const;

	private:
		bool m_IsContinue;
	};

	class WhileStatement : public Statement {
	public:
		WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<BlockExpression> body);
		Value Accept(Visitor* visitor) const override;

		Expression const* Condition() const;
		BlockExpression const* Body() const;

	private:
		std::unique_ptr<Expression> m_Condition;
		std::unique_ptr<BlockExpression> m_Body;
	};

	class ReturnStatement : public Statement {
	public:
		explicit ReturnStatement(std::unique_ptr<Expression> expr = nullptr);
		Value Accept(Visitor* visitor) const override;
		Expression const* ReturnValue() const;

	private:
		std::unique_ptr<Expression> m_Expr;
	};

	class IfThenElseExpression : public Expression {
	public:
		IfThenElseExpression(std::unique_ptr<Expression> condition, std::unique_ptr<Expression> thenExpr, std::unique_ptr<Expression> elseExpr = nullptr);
		Value Accept(Visitor* visitor) const override;

		Expression const* Condition() const;
		Expression const* Then() const;
		Expression const* Else() const;

	private:
		std::unique_ptr<Expression> m_Condition;
		std::unique_ptr<Expression> m_Then, m_Else;
	};

	class FunctionDeclaration : public Statement {
	public:
		FunctionDeclaration(std::string name, std::vector<std::string> parameters, std::unique_ptr<Expression> body);
		Value Accept(Visitor* visitor) const override;

		std::string const& Name() const;
		std::vector<std::string> const& Parameters() const;
		Expression const* Body() const;

	private:
		std::string m_Name;
		std::vector<std::string> m_Parameters;
		std::unique_ptr<Expression> m_Body;
	};

	class PostfixExpression : public Expression {
	public:
		PostfixExpression(std::unique_ptr<Expression> expr, Token token);
		Value Accept(Visitor* visitor) const override;

		Token const& Operator() const;
		Expression const* Argument() const;

	private:
		std::unique_ptr<Expression> m_Expr;
		Token m_Token;
	};

	class BinaryExpression : public Expression {
	public:
		BinaryExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
		Value Accept(Visitor* visitor) const override;

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
		Value Accept(Visitor* visitor) const override;
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
		Value Accept(Visitor* visitor) const override;

		std::string ToString() const override;
		Token const& Literal() const;

	private:
		Token m_Token;
	};

	class NameExpression : public Expression {
	public:
		explicit NameExpression(std::string name);
		NodeType Type() const override {
			return NodeType::Name;
		}
		Value Accept(Visitor* visitor) const override;
		std::string const& Name() const;
		std::string ToString() const override;

	private:
		std::string m_Name;
	};

	class InvokeFunctionExpression : public Expression {
	public:
		InvokeFunctionExpression(std::string name, std::vector<std::unique_ptr<Expression>> args);
		Value Accept(Visitor* visitor) const override;
		std::string const& Name() const;
		std::vector<std::unique_ptr<Expression>> const& Arguments() const;

	private:
		std::string m_Name;
		std::vector<std::unique_ptr<Expression>> m_Arguments;
	};

	class ForStatement : public Statement {
	public:
		ForStatement(std::unique_ptr<Statement> init, std::unique_ptr<Expression> whileExpr, std::unique_ptr<Expression> incExpr, std::unique_ptr<BlockExpression> body);
		Value Accept(Visitor* visitor) const override;

		Statement const* Init() const;
		Expression const* While() const;
		Expression const* Inc() const;
		BlockExpression const* Body() const;

	private:
		std::unique_ptr<Expression> m_While, m_Inc;
		std::unique_ptr<Statement> m_Init;
		std::unique_ptr<BlockExpression> m_Body;
	};

	class AnonymousFunctionExpression : public Expression {
	public:
		AnonymousFunctionExpression(std::vector<std::string> args, std::unique_ptr<Expression> body);
		Value Accept(Visitor* visitor) const override;
		std::vector<std::string> const& Args() const;
		Expression const* Body() const;

	private:
		std::vector<std::string> m_Args;
		std::unique_ptr<Expression> m_Body;
	};

}
