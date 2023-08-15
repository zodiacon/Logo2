#include "pch.h"
#include "Value.h"
#include <Errors.h>

namespace Logo2 {
	bool Value::IsInteger() const {
		return m_Value.index() == 0;
	}

	bool Value::IsBoolean() const {
		return m_Value.index() == 2;
	}

	bool Value::IsReal() const {
		return m_Value.index() == 1;
	}

	float Value::ToFloat() const {
		switch (m_Value.index()) {
			case 0: return (float)Integer();
			case 1: return (float)Real();
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	long long Value::ToInteger() const {
		switch (m_Value.index()) {
			case 0: return Integer();
			case 1: return (long long)Real();
			case 2: return Boolean() ? 1 : 0;
		}
		throw RuntimeError(ErrorType::TypeMismatch);
	}

	Value Value::operator-() const {
		switch (m_Value.index()) {
			case 0:	return -Integer();
			case 1:	return -Real();
		}
		return Value();
	}

	bool Value::operator==(Value const& other) {
		switch (Index() | (other.Index() << 4)) {
			case 0: return Integer() == other.Integer();
			case 1: return Real() == other.Integer();
			case 16: return Integer() == other.Real();
			case 17: return Real() == other.Real();
		}
		return false;
	}

	bool Value::operator!=(Value const& other) {
		return !(*this == other);
	}

	long long Value::Integer() const {
		return std::get<0>(m_Value);
	}

	double Value::Real() const {
		return std::get<1>(m_Value);
	}

	bool Value::Boolean() const {
		return std::get<2>(m_Value);
	}

	std::string const& Value::String() const {
		return std::get<3>(m_Value);
	}

	int Value::Index() const {
		return (int)m_Value.index();
	}

	std::string Value::ToString() const {
		switch (m_Value.index()) {
			case 0: return std::to_string(Integer());
			case 1: return std::to_string(Real());
			case 2: return Boolean() ? "true" : "false";
			case 3: return String();
		}
		return std::string();
	}

	Value operator+(Value const& left, Value const& right) {
		switch (left.Index() | (right.Index() << 4)) {
			case 0: return left.Integer() + right.Integer();
			case 1: return left.Real() + right.Integer();
			case 16: return left.Integer() + right.Real();
			case 17: return left.Real() + right.Real();
		}
		return Value();
	}

	Value operator-(Value const& left, Value const& right) {
		switch (left.Index() | (right.Index() << 4)) {
			case 0: return left.Integer() - right.Integer();
			case 1: return left.Real() - right.Integer();
			case 16: return left.Integer() - right.Real();
			case 17: return left.Real() - right.Real();
		}
		return Value();
	}

	Value operator*(Value const& left, Value const& right) {
		switch (left.Index() | (right.Index() << 4)) {
			case 0: return left.Integer() * right.Integer();
			case 1: return left.Real() * right.Integer();
			case 16: return left.Integer() * right.Real();
			case 17: return left.Real() * right.Real();
		}
		return Value();
	}

	Value operator/(Value const& left, Value const& right) {
		switch (left.Index() | (right.Index() << 4)) {
			case 0: return left.Integer() / right.Integer();
			case 1: return left.Real() / right.Integer();
			case 16: return left.Integer() / right.Real();
			case 17: return left.Real() / right.Real();
		}
		return Value();
	}

	Value operator%(Value const& left, Value const& right) {
		switch (left.Index() | (right.Index() << 4)) {
			case 0: return left.Integer() % right.Integer();
		}
		return Value();
	}

	std::partial_ordering operator<=>(Value const& left, Value const& right) {
		switch (left.Index() | (right.Index() << 4)) {
			case 0: return left.Integer() <=> right.Integer();
			case 1: return left.Real() <=> right.Integer();
			case 16: return left.Integer() <=> right.Real();
			case 17: return left.Real() <=> right.Real();
		}
		return std::partial_ordering::unordered;
	}
}
