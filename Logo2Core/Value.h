#pragma once

struct Value {
	Value(double d) : m_Value(d) {}
	Value(long long n) : m_Value(n) {}
	Value(bool b) : m_Value(b) {}
	Value(nullptr_t = nullptr) : m_Value(nullptr) {}
	Value(std::string s) : m_Value(std::move(s)) {}

	friend Value operator+(Value const& left, Value const& right);
	friend Value operator-(Value const& left, Value const& right);
	friend Value operator*(Value const& left, Value const& right);
	friend Value operator/(Value const& left, Value const& right);
	friend Value operator%(Value const& left, Value const& right);
	Value operator-() const;
	friend std::partial_ordering operator<=>(Value const& left, Value const& right);
	bool operator==(Value const& other);
	bool operator!=(Value const& other);

	long long Integer() const;
	double Real() const;
	bool Boolean() const;
	std::string const& String() const;

	int Index() const;
	std::string ToString() const;

private:
	std::variant<long long, double, bool, std::string, nullptr_t> m_Value;
};

