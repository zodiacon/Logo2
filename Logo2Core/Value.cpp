#include "pch.h"
#include "Value.h"
#include <Errors.h>

#define OPS(op1, op2) (op1 | (op2 << 4))

namespace Logo2 {
	Value::Value(std::shared_ptr<Function> f) : m_Value(f) {
	}

	Value::operator bool() const {
		return m_Value.index() != 0;
	}

	bool Value::IsInteger() const {
		return m_Value.index() == 0;
	}

	bool Value::IsBoolean() const {
		return m_Value.index() == 2;
	}

	bool Value::IsReal() const {
		return m_Value.index() == 1;
	}

	bool Value::IsFunction() const {
		return m_Value.index() == 5;
	}

	float Value::ToFloat() const {
		return (float)ToDouble();
	}

	bool Value::ToBoolean() const {
		switch (Index()) {
			case TypeInteger: return Integer() != 0;
			case TypeBoolean: return Boolean();
			case TypeReal: return Real() != 0.0;
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	double Value::ToDouble() const {
		switch (Index()) {
			case TypeInteger: return (double)Integer();
			case TypeReal: return Real();
			case TypeBoolean: return Boolean() ? 1.0 : 0.0;
			case TypeString: return std::stof(String());
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	long long Value::ToInteger() const {
		switch (Index()) {
			case TypeInteger: return Integer();
			case TypeReal: return (long long)Real();
			case TypeBoolean: return Boolean() ? 1 : 0;
			case TypeString: return strtoll(String().c_str(), nullptr, 0);
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	Value Value::operator-() const {
		switch (m_Value.index()) {
			case TypeInteger:	return -Integer();
			case TypeReal:	return -Real();
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	Value Value::operator!() const {
		switch (m_Value.index()) {
			case TypeInteger:	return Integer() ? 0 : 1LL;
			case TypeReal:	return Real() ? 0.0 : 1.0;
			case TypeBoolean: return !Boolean();
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	Value Value::operator~() const {
		switch (m_Value.index()) {
			case TypeInteger:	return ~Integer();
			case TypeBoolean: return !Boolean();
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	Value Value::Power(Value const& right) const {
		switch (Index() | (right.Index() << 4)) {
			case TypeInteger | (TypeInteger << 4): return (long long)std::pow(Integer(), right.Integer());
			case TypeReal | (TypeInteger << 4): return std::pow(Real(), right.Integer());
			case TypeInteger | (TypeReal << 4) : return std::pow(Integer(), right.Real());
			case TypeReal | (TypeReal << 4) : return std::pow(Real(), right.Real());
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	bool Value::operator==(Value const& other) const {
		switch (Index() | (other.Index() << 4)) {
			case TypeInteger | (TypeInteger << 4): return Integer() == other.Integer();
			case TypeReal | (TypeInteger << 4): return Real() == other.Integer();
			case TypeInteger | (TypeReal << 4): return Integer() == other.Real();
			case TypeReal | (TypeReal << 4): return Real() == other.Real();
			case TypeNull | (TypeNull << 4) : return true;
		}
		return false;
	}

	long long Value::Integer() const {
		return std::get<TypeInteger>(m_Value);
	}

	double Value::Real() const {
		return std::get<TypeReal>(m_Value);
	}

	bool Value::Boolean() const {
		return std::get<TypeBoolean>(m_Value);
	}

	std::string const& Value::String() const {
		return std::get<TypeString>(m_Value);
	}

	Function const* const Value::Func() const {
		return std::get<TypeFunction>(m_Value).get();
	}

	Value::TypeIndex Value::Index() const {
		return (TypeIndex)m_Value.index();
	}

	std::string Value::ToString() const {
		switch (m_Value.index()) {
			case TypeInteger: return std::to_string(Integer());
			case TypeReal: return std::to_string(Real());
			case TypeBoolean: return Boolean() ? "true" : "false";
			case TypeString: return String();
		}
		return std::string();
	}

	Value Value::operator+(Value const& right) const {
		switch (Index() | (right.Index() << 4)) {
			case TypeInteger | (TypeInteger << 4): return Integer() + right.Integer();
			case TypeReal | (TypeInteger << 4): return Real() + right.Integer();
			case TypeInteger | (TypeReal << 4): return Integer() + right.Real();
			case TypeReal | (TypeReal << 4): return Real() + right.Real();
			case TypeString | (TypeString << 4) : return String() + right.String();
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	Value Value::operator-(Value const& right) const {
		return (*this) + -right;
	}

	Value Value::operator*(Value const& right) const {
		switch (Index() | (right.Index() << 4)) {
			case TypeInteger | (TypeInteger << 4): return Integer() * right.Integer();
			case TypeReal | (TypeInteger << 4): return Real() * right.Integer();
			case TypeInteger | (TypeReal << 4): return Integer() * right.Real();
			case TypeReal | (TypeReal << 4): return Real() * right.Real();
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	Value Value::operator&(Value const& right) const {
		switch (Index() | (right.Index() << 4)) {
			case TypeInteger: return Integer() & right.Integer();
			case TypeBoolean | (TypeBoolean << 4) : return bool(Boolean() & right.Boolean());
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	Value Value::operator|(Value const& right) const {
		switch (Index() | (right.Index() << 4)) {
			case TypeInteger: return Integer() | right.Integer();
			case TypeBoolean | (TypeBoolean << 4) : return bool(Boolean() | right.Boolean());
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	Value Value::operator^(Value const& right) const {
		switch (Index() | (right.Index() << 4)) {
			case TypeInteger | (TypeInteger << 4): return Integer() ^ right.Integer();
			case TypeBoolean | (TypeBoolean << 4) : return bool(Boolean() ^ right.Boolean());
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	Value Value::operator/(Value const& right) const {
		switch (right.Index()) {
			case TypeInteger: 				
				if (right.Integer() == 0)
					throw RuntimeError(ErrorType::DivisionByZero);
				break;
			case TypeReal:
				if(right.Real() == 0)
					throw RuntimeError(ErrorType::DivisionByZero);
				break;
			default:
				throw RuntimeError(ErrorType::TypeMismatch);
		}
		switch (Index() | (right.Index() << 4)) {
			case TypeInteger | (TypeInteger << 4) : return Integer() / right.Integer();
			case TypeReal | (TypeInteger << 4) : return Real() / right.Integer();
			case TypeInteger | (TypeReal << 4): return Integer() / right.Real();
			case TypeReal | (TypeReal << 4): return Real() / right.Real();
		}
		return Value();
	}

	Value Value::operator%(Value const& right) const {
		switch (Index() | (right.Index() << 4)) {
			case TypeInteger | (TypeInteger << 4) : return Integer() % right.Integer();
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	std::partial_ordering Value::operator<=>(Value const& right) const {
		switch (Index() | (right.Index() << 4)) {
			case TypeInteger | (TypeInteger << 4) : return Integer() <=> right.Integer();
			case TypeReal | (TypeInteger << 4) : return Real() <=> right.Integer();
			case TypeBoolean | (TypeInteger << 4) : return (int)Boolean() <=> right.Integer();
			case TypeInteger | (TypeReal << 4): return Integer() <=> right.Real();
			case TypeReal | (TypeReal << 4): return Real() <=> right.Real();
			case TypeInteger | (TypeBoolean << 4): return Integer() <=> (int)right.Boolean();
			case TypeBoolean | (TypeBoolean << 4) : return Boolean() <=> right.Boolean();
			case TypeString | (TypeString << 4) : return String() <=> right.String();
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

}
