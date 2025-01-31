#pragma once

#include "Logo2Ast.h"
#include "Value.h"
#include "Logo2Core.h"
#include <stack>
#include "Visitor.h"
#include "TypeObject.h"

namespace Logo2 {
	class Interpreter;

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
		std::unique_ptr<Scope> Clone(Scope* parent = nullptr) const;

	private:
		std::unordered_map<std::string, Variable> m_Variables;
		Scope* m_Parent;
	};

	struct Return {
		Expression const* ReturnValue;
	};

	struct QuitAppException {
		int ExitCode;
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
		Value VisitAnonymousFunction(AnonymousFunctionExpression const* func) override;
		Value VisitEnumDeclaration(EnumDeclaration const* decl) override;

		bool AddNativeFunction(std::string name, int arity, NativeFunction f);
		bool AddVariable(std::string name, Variable var);
		Variable const* FindVariable(std::string const& name) const;
		Variable* FindVariable(std::string const& name);
		Value InvokeFunction(Function const& f, InvokeFunctionExpression const* expr);

	private:
		enum class LoopResult {
			None,
			Break,
			Continue
		};
		void PushScope();
		void PopScope();

		std::stack<std::unique_ptr<Scope>> m_Scopes;
		std::unordered_map<std::string, Function> m_Functions;
		LoopResult m_LoopResult{ LoopResult::None };
		std::unordered_map<std::string, TypeObject> m_Types;
	};

	DEFINE_ENUM_FLAG_OPERATORS(Logo2::VariableFlags);
};
