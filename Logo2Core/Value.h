#pragma once

#include <functional>
#include <vector>

namespace Logo2 {
	class Expression;
	class Interpreter;
	struct Value;
	struct Scope;

	struct EnumType {
		explicit EnumType(std::string name);

	private:
		std::unordered_map<std::string, long long> m_Values;
	};

	struct EnumTypeObject {
		explicit EnumTypeObject(EnumType& type);

	};

	struct UserType {
	};

	struct UserObject {
		explicit UserObject(UserType& type);
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
		Value(nullptr_t = nullptr) : m_Value(nullptr) {}
		Value(std::string s) : m_Value(std::move(s)) {}
		Value(std::shared_ptr<Function> f);

		bool IsInteger() const;
		bool IsBoolean() const;
		bool IsReal() const;
		bool IsFunction() const;

		float ToFloat() const;
		bool ToBoolean() const;
		double ToDouble() const;
		long long ToInteger() const;

		friend Value operator+(Value const& left, Value const& right);
		friend Value operator-(Value const& left, Value const& right);
		friend Value operator*(Value const& left, Value const& right);
		friend Value operator/(Value const& left, Value const& right);
		friend Value operator%(Value const& left, Value const& right);
		friend Value operator&(Value const& left, Value const& right);
		friend Value operator|(Value const& left, Value const& right);
		friend Value operator^(Value const& left, Value const& right);
		Value operator-() const;
		Value operator!() const;
		Value operator~() const;
		friend std::partial_ordering operator<=>(Value const& left, Value const& right);
		bool operator==(Value const& other) const;

		long long Integer() const;
		double Real() const;
		bool Boolean() const;
		std::string const& String() const;
		Function const* const Func() const;

		int Index() const;
		std::string ToString() const;

	private:
		std::variant<long long, double, bool, std::string, nullptr_t, std::shared_ptr<Function>, EnumTypeObject, std::shared_ptr<UserObject>> m_Value;
	};
}
