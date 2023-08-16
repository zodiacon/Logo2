#pragma once

namespace Logo2 {
	class Parser;
	class Expression;
	struct Token;

	struct InfixParslet abstract {
		virtual std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) = 0;
		virtual int Precedence() const = 0;
	};

	struct PrefixParslet abstract {
		virtual std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) = 0;
		virtual int Precedence() const {
			return 0;
		}
	};

	struct NumberParslet : PrefixParslet {
		std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
	};

	struct NameParslet : PrefixParslet {
		std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
	};

	struct GroupParslet : PrefixParslet {
		std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
		int Precedence() const override;
	};

	struct PrefixOperatorParslet : PrefixParslet {
		explicit PrefixOperatorParslet(int precedence);

		std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
		int Precedence() const override;

	private:
		int m_Precedence;
	};

	struct PostfixOperatorParslet : InfixParslet {
		explicit PostfixOperatorParslet(int precedence);

		std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) override;
		int Precedence() const override;

	private:
		int m_Precedence;
	};

	struct InvokeFunctionParslet : PostfixOperatorParslet {
		InvokeFunctionParslet();
		std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) override;
	};

	struct BinaryOperatorParslet : InfixParslet {
		explicit BinaryOperatorParslet(int precedence, bool right = false);
		std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) override;
		int Precedence() const override;

	private:
		int m_Precedence;
		bool m_RightAssoc;
	};

	struct AssignParslet : InfixParslet {
		std::unique_ptr<Expression> Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) override;
		int Precedence() const override;
	};

	struct IfThenElseParslet : PrefixParslet {
		std::unique_ptr<Expression> Parse(Parser& parser, Token const& token) override;
	};
}
