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
		explicit RuntimeError(ErrorType type, LogoAstNode const* node = nullptr, std::string text = "") : Error(type), Node(node), ErrorText(std::move(text)) {}

		ErrorType Error;
		LogoAstNode const* Node;
		std::string ErrorText;
	};
}
