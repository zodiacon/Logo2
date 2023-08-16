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
	};

	struct RuntimeError {
		ErrorType Error;
		LogoAstNode const* Node;
	};
}
