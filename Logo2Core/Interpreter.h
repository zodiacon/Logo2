#pragma once

#include "Logo2Ast.h"
#include "Value.h"
#include "Logo2Core.h"
#include <functional>

namespace Logo2 {
	using NativeFunction = std::function<Value(Interpreter&, std::vector<Value>&)>;

	class Interpreter {
	public:
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

		bool AddNativeFunction(std::string name, int arity, NativeFunction f);

		enum class VariableFlags {
			None,
			Const = 1,
			Static = 2,
		};

	private:
		struct Variable {
			Value VarValue;
			VariableFlags Flags;
		};
		struct Function {
			int ArgCount;
			std::unique_ptr<BlockExpression> Code;
			NativeFunction NativeCode;
		};
		std::unordered_map<std::string, Variable> m_Variables;
		std::unordered_map<std::string, Function> m_Functions;
	};

	DEFINE_ENUM_FLAG_OPERATORS(Logo2::Interpreter::VariableFlags);
};
