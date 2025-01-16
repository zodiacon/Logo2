#pragma once

#include <functional>
#include <vector>

namespace Logo2 {
	class Expression;
	class Interpreter;
	struct Value;
	struct Scope;

	class TypeObject;

	struct UserObject {
		explicit UserObject(TypeObject& type);
	};

	using NativeFunction = std::function<Value(Interpreter&, std::vector<Value>&)>;

	struct Function {
		int ArgCount;
		Expression const* Code{ nullptr };
		NativeFunction NativeCode;
		std::vector<std::string> Parameters;
		std::unique_ptr<Scope> Environment;
	};

	struct Value {
		Value(double d) : m_Value(d) {}
		Value(long long n) : m_Value(n) {}
		Value(bool b) : m_Value(b) {}
		Value() = default;
		Value(std::string s) : m_Value(std::move(s)) {}
		Value(std::shared_ptr<Function> f);

		enum TypeIndex {
			TypeNull, TypeInteger, TypeReal, TypeBoolean, TypeString, TypeFunction, TypeUserObject
		};

		operator bool() const;
		bool IsInteger() const;
		bool IsBoolean() const;
		bool IsReal() const;
		bool IsFunction() const;

		float ToFloat() const;
		bool ToBoolean() const;
		double ToDouble() const;
		long long ToInteger() const;

		Value operator+(Value const& right) const;
		Value operator-(Value const& right) const;
		Value operator*(Value const& right) const;
		Value operator/(Value const& right) const;
		Value operator%(Value const& right) const;
		Value operator&(Value const& right) const;
		Value operator|(Value const& right) const;
		Value operator^(Value const& right) const;
		Value operator-() const;
		Value operator!() const;
		Value operator~() const;
		Value Power(Value const& right) const;

		std::partial_ordering operator<=>(Value const& right) const;
		bool operator==(Value const& other) const;

		long long Integer() const;
		double Real() const;
		bool Boolean() const;
		std::string const& String() const;
		Function const* const Func() const;

		TypeIndex Index() const;
		std::string ToString() const;

	private:
		std::variant<std::monostate, long long, double, bool, std::string, std::shared_ptr<Function>, std::shared_ptr<UserObject>> m_Value;
	};
}
