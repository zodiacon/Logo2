#pragma once

#include "Logo2Ast.h"
#include "Value.h"
#include "Logo2Core.h"
#include <functional>
#include <stack>

namespace Logo2 {
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

	class Interpreter {
	public:
		Interpreter();
		Value Eval(LogoAstNode const* node);

		Value VisitLiteral(LiteralExpression const* expr);
		Value VisitBinary(BinaryExpression const* expr);
		Value VisitUnary(UnaryExpression const* expr);
		Value VisitName(NameExpression const* expr);
		Value VisitBlock(BlockExpression const* expr);
		Value VisitVar(VarStatement const* expr);
		Value VisitAssign(AssignExpression const* expr);
		Value VisitPostfix(PostfixExpression const* expr);
		Value VisitInvokeFunction(InvokeFunctionExpression const* expr);
		Value VisitRepeat(RepeatStatement const* expr);
		Value VisitWhile(WhileStatement const* stmt);
		Value VisitIfThenElse(IfThenElseExpression const* expr);
		Value VisitFunctionDeclaration(FunctionDeclaration const* decl);

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
