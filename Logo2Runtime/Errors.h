#pragma once

namespace Logo2 {
	class LogoAstNode;

	enum class ErrorType {
		Success,
		CannotAssignConst,
		TypeMismatch,
		ArgumentCountMismatch,
		UndefinedFunction,
		DivisionByZero,
		UndefinedOperator,
		UndefinedSymbol,
		NotCallable,
	};

	struct RuntimeError {
		RuntimeError(ErrorType type, LogoAstNode const* node = nullptr) : Error(type), Node(node) {}

		ErrorType Error;
		LogoAstNode const* Node;
	};
}
