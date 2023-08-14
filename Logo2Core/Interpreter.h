#pragma once

#include "Logo2Ast.h"
#include "Value.h"
#include "Logo2Core.h"
#include <functional>
#include <Runtime.h>

class Interpreter {
public:
	Interpreter();

	Value VisitLiteral(LiteralExpression const* expr);
	Value VisitBinary(BinaryExpression const* expr);
	Value VisitUnary(UnaryExpression const* expr);
	Value VisitName(NameExpression const* expr);
	Value VisitBlock(BlockExpression const* expr);
	Value VisitVar(VarStatement const* expr);
	Value VisitAssign(AssignExpression const* expr);
	Value VisitPostfix(PostfixExpression const* expr);
	Value VisitInvokeFunction(InvokeFunctionExpression const* expr);

	enum class VariableFlags {
		None,
		Const = 1,
		Static = 2,
	};

	Runtime& GetRuntime();

private:
	struct Variable {
		Value VarValue;
		VariableFlags Flags;
	};
	struct Function {
		int ArgCount;
		std::unique_ptr<BlockExpression> Code;
		std::function<Value(Interpreter&, std::vector<Value>&)> NativeCode;
	};
	std::unordered_map<std::string, Variable> m_Variables;
	std::unordered_map<std::string, Function> m_Functions;
	Runtime m_Runtime;
};

DEFINE_ENUM_FLAG_OPERATORS(Interpreter::VariableFlags);

