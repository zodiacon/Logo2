#include "pch.h"
#include "Logo2Ast.h"
#include "Interpreter.h"

BinaryExpression::BinaryExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right) 
	: m_Left(std::move(left)), m_Right(std::move(right)), m_Operator(std::move(op)) {
}

Value BinaryExpression::Accept(Interpreter* visitor) const {
	return visitor->VisitBinary(this);
}

std::string BinaryExpression::ToString() const {
	return std::format("({}{}{})", m_Left->ToString(), m_Operator.Lexeme, m_Right->ToString());
}

Expression* BinaryExpression::Left() const {
	return m_Left.get();
}

Expression* BinaryExpression::Right() const {
	return m_Right.get();
}

Token const& BinaryExpression::Operator() const {
	return m_Operator;
}

PostfixExpression::PostfixExpression(std::unique_ptr<Expression> expr, Token token)
	: m_Expr(std::move(expr)), m_Token(token) {
}

Value PostfixExpression::Accept(Interpreter* visitor) const {
	return visitor->VisitPostfix(this);
}

LiteralExpression::LiteralExpression(Token token) : m_Token(std::move(token)) {
}

Value LiteralExpression::Accept(Interpreter* visitor) const {
	return visitor->VisitLiteral(this);
}

std::string LiteralExpression::ToString() const {
	return m_Token.Lexeme;
}

Token const& LiteralExpression::Literal() const {
	return m_Token;
}

NameExpression::NameExpression(std::string name) : m_Name(std::move(name)) {
}

Value NameExpression::Accept(Interpreter* visitor) const {
	return visitor->VisitName(this);
}

std::string const& NameExpression::Name() const {
	return m_Name;
}

NodeType NameExpression::Type() const {
	return NodeType::Name;
}

std::string NameExpression::ToString() const {
	return m_Name;
}

UnaryExpression::UnaryExpression(Token op, std::unique_ptr<Expression> arg) : m_Arg(std::move(arg)), m_Operator(std::move(op)) {
}

Value UnaryExpression::Accept(Interpreter* visitor) const {
	return visitor->VisitUnary(this);
}

std::string UnaryExpression::ToString() const {
	return std::format("({}{})", m_Operator.Lexeme, m_Arg->ToString());
}

Token const& UnaryExpression::Operator() const {
	return m_Operator;
}

Expression* UnaryExpression::Arg() const {
	return m_Arg.get();
}

void BlockExpression::Add(std::unique_ptr<LogoAstNode> node) {
	m_Stmts.push_back(std::move(node));
}

Value BlockExpression::Accept(Interpreter* visitor) const {
	return visitor->VisitBlock(this);
}

std::vector<LogoAstNode*> const BlockExpression::Expressions() const {
	std::vector<LogoAstNode*> expr;
	for (auto& e : m_Stmts)
		expr.push_back(e.get());
	return expr;
}

std::string BlockExpression::ToString() const {
	std::string result;
	for (auto& e : m_Stmts)
		result += e->ToString() + "\n";
	return result.substr(0, result.length() - 1);
}

VarStatement::VarStatement(std::string name, bool isConst, std::unique_ptr<Expression> init) 
	: m_Name(std::move(name)), m_Init(std::move(init)), m_IsConst(isConst) {
}

Value VarStatement::Accept(Interpreter* visitor) const {
	return visitor->VisitVar(this);
}

std::string VarStatement::ToString() const {
	return std::format("{} = {};", m_Name, m_Init ? m_Init->ToString() : "");
}

std::string const& VarStatement::Name() const {
	return m_Name;
}

Expression const* VarStatement::Init() const {
	return m_Init.get();
}

bool VarStatement::IsConst() const {
	return m_IsConst;
}

AssignExpression::AssignExpression(std::string name, std::unique_ptr<Expression> expr) 
	: m_Name(std::move(name)), m_Expr(std::move(expr)) {
}

Value AssignExpression::Accept(Interpreter* visitor) const {
	return visitor->VisitAssign(this);
}

std::string const& AssignExpression::Variable() const {
	return m_Name;
}

Expression* const AssignExpression::Value() const {
	return m_Expr.get();
}

InvokeFunctionExpression::InvokeFunctionExpression(std::string name, std::vector<std::unique_ptr<Expression>> args) :
	m_Name(std::move(name)), m_Arguments(std::move(args)) {
}

Value InvokeFunctionExpression::Accept(Interpreter* visitor) const {
	return visitor->VisitInvokeFunction(this);
}

std::string const& InvokeFunctionExpression::Name() const {
	return m_Name;
}

std::vector<std::unique_ptr<Expression>> const& InvokeFunctionExpression::Arguments() const {
	return m_Arguments;
}

RepeatStatement::RepeatStatement(std::unique_ptr<Expression> count, std::unique_ptr<BlockExpression> body) : 
	m_Count(std::move(count)), m_Block(std::move(body)) {
}

Value RepeatStatement::Accept(Interpreter* visitor) const {
	return visitor->VisitRepeat(this);
}

Expression const* RepeatStatement::Count() const {
	return m_Count.get();
}

BlockExpression const* RepeatStatement::Block() const {
	return m_Block.get();
}

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expr) : m_Expr(std::move(expr)) {
}

Value ExpressionStatement::Accept(Interpreter* visitor) const {
	return m_Expr->Accept(visitor);
}

Expression const* ExpressionStatement::Expr() const {
	return m_Expr.get();
}
