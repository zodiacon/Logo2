#pragma once

#include "Logo2Ast.h"
#include "Value.h"
#include "Logo2Core.h"
#include <functional>
#include <stack>
#include "Visitor.h"

namespace Logo2 {
	class Interpreter;

	using NativeFunction = std::function<Value(Interpreter&, std::vector<Value>&)>;

	enum class VariableFlags {
		None,
		Const = 1,
		Static = 2,
	};

	struct Variable {
		Value VarValue;
		VariableFlags Flags;
	};

	struct Scope {
		explicit Scope(Scope* parent = nullptr);
		bool AddVariable(std::string name, Variable var);
		Variable const* FindVariable(std::string const& name) const;
		Variable* FindVariable(std::string const& name);

	private:
		std::unordered_map<std::string, Variable> m_Variables;
		Scope* m_Parent;
	};

	struct Return {
		Expression const* ReturnValue;
	};

	struct BreakOrContinue {
		bool Continue;
	};

	class Interpreter : public Visitor {
	public:
		Interpreter();
		Value Eval(LogoAstNode const* node);

		Value VisitLiteral(LiteralExpression const* expr) override;
		Value VisitBinary(BinaryExpression const* expr) override;
		Value VisitUnary(UnaryExpression const* expr) override;
		Value VisitName(NameExpression const* expr)override;
		Value VisitBlock(BlockExpression const* expr) override;
		Value VisitVar(VarStatement const* expr) override;
		Value VisitAssign(AssignExpression const* expr) override;
		Value VisitPostfix(PostfixExpression const* expr) override;
		Value VisitInvokeFunction(InvokeFunctionExpression const* expr) override;
		Value VisitRepeat(RepeatStatement const* expr) override;
		Value VisitWhile(WhileStatement const* stmt)override;
		Value VisitIfThenElse(IfThenElseExpression const* expr) override;
		Value VisitFunctionDeclaration(FunctionDeclaration const* decl) override;
		Value VisitReturn(ReturnStatement const* expr) override;
		Value VisitBreakContinue(BreakOrContinueStatement const* stmt) override;
		Value VisitFor(ForStatement const* stmt) override;
		Value VisitStatements(Statements const* stmts) override;

		bool AddNativeFunction(std::string name, int arity, NativeFunction f);
		bool AddVariable(std::string name, Variable var);
		Variable const* FindVariable(std::string const& name) const;
		Variable* FindVariable(std::string const& name);

	private:
		void PushScope();
		void PopScope();

		struct Function {
			int ArgCount;
			BlockExpression const* Code{ nullptr };
			NativeFunction NativeCode;
			std::vector<std::string> Parameters;
		};
		std::stack<std::unique_ptr<Scope>> m_Scopes;
		std::unordered_map<std::string, Function> m_Functions;
	};

	DEFINE_ENUM_FLAG_OPERATORS(Logo2::VariableFlags);
};
