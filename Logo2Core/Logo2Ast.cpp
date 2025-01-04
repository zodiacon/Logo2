#include "pch.h"
#include "Logo2Ast.h"
#include "Interpreter.h"

using namespace Logo2;
using namespace std;

BinaryExpression::BinaryExpression(unique_ptr<Expression> left, Token op, unique_ptr<Expression> right) 
	: m_Left(move(left)), m_Right(move(right)), m_Operator(move(op)) {
}

Value BinaryExpression::Accept(Visitor* visitor) const {
	return visitor->VisitBinary(this);
}

string BinaryExpression::ToString() const {
	return format("({}{}{})", m_Left->ToString(), m_Operator.Lexeme, m_Right->ToString());
}

Expression* BinaryExpression::Left() const {
	return m_Left.get();
}

Expression* BinaryExpression::Right() const {
	return m_Right.get();
}

Logo2::Token const& BinaryExpression::Operator() const {
	return m_Operator;
}

PostfixExpression::PostfixExpression(unique_ptr<Expression> expr, Token token)
	: m_Expr(move(expr)), m_Token(move(token)) {
}

Value PostfixExpression::Accept(Visitor* visitor) const {
	return visitor->VisitPostfix(this);
}

LiteralExpression::LiteralExpression(Token token) : m_Token(move(token)) {
}

Value LiteralExpression::Accept(Visitor* visitor) const {
	return visitor->VisitLiteral(this);
}

string LiteralExpression::ToString() const {
	return m_Token.Lexeme;
}

Token const& LiteralExpression::Literal() const {
	return m_Token;
}

NameExpression::NameExpression(string name) : m_Name(move(name)) {
}

Value NameExpression::Accept(Visitor* visitor) const {
	return visitor->VisitName(this);
}

string const& NameExpression::Name() const {
	return m_Name;
}

string NameExpression::ToString() const {
	return m_Name;
}

UnaryExpression::UnaryExpression(Token op, unique_ptr<Expression> arg) : m_Arg(move(arg)), m_Operator(move(op)) {
}

Value UnaryExpression::Accept(Visitor* visitor) const {
	return visitor->VisitUnary(this);
}

string UnaryExpression::ToString() const {
	return format("({}{})", m_Operator.Lexeme, m_Arg->ToString());
}

Token const& UnaryExpression::Operator() const {
	return m_Operator;
}

Expression* UnaryExpression::Arg() const {
	return m_Arg.get();
}

void BlockExpression::Add(unique_ptr<LogoAstNode> node) {
	m_Stmts.push_back(move(node));
}

Value BlockExpression::Accept(Visitor* visitor) const {
	return visitor->VisitBlock(this);
}

vector<LogoAstNode*> const BlockExpression::Expressions() const {
	vector<LogoAstNode*> expr;
	for (auto& e : m_Stmts)
		expr.push_back(e.get());
	return expr;
}

string BlockExpression::ToString() const {
	string result;
	for (auto& e : m_Stmts)
		result += e->ToString() + "\n";
	return result.substr(0, result.length() - 1);
}

VarStatement::VarStatement(string name, bool isConst, unique_ptr<Expression> init) 
	: m_Name(move(name)), m_Init(move(init)), m_IsConst(isConst) {
}

Value VarStatement::Accept(Visitor* visitor) const {
	return visitor->VisitVar(this);
}

string VarStatement::ToString() const {
	return format("{} = {};", m_Name, m_Init ? m_Init->ToString() : "");
}

string const& VarStatement::Name() const {
	return m_Name;
}

Expression const* VarStatement::Init() const {
	return m_Init.get();
}

bool VarStatement::IsConst() const {
	return m_IsConst;
}

AssignExpression::AssignExpression(string name, unique_ptr<Expression> expr) 
	: m_Name(move(name)), m_Expr(move(expr)) {
}

Value AssignExpression::Accept(Visitor* visitor) const {
	return visitor->VisitAssign(this);
}

string const& AssignExpression::Variable() const {
	return m_Name;
}

Expression* const AssignExpression::Value() const {
	return m_Expr.get();
}

InvokeFunctionExpression::InvokeFunctionExpression(string name, vector<unique_ptr<Expression>> args) :
	m_Name(move(name)), m_Arguments(move(args)) {
}

Value InvokeFunctionExpression::Accept(Visitor* visitor) const {
	return visitor->VisitInvokeFunction(this);
}

string const& InvokeFunctionExpression::Name() const {
	return m_Name;
}

vector<unique_ptr<Expression>> const& InvokeFunctionExpression::Arguments() const {
	return m_Arguments;
}

RepeatStatement::RepeatStatement(unique_ptr<Expression> count, unique_ptr<BlockExpression> body) : 
	m_Count(move(count)), m_Block(move(body)) {
}

Value RepeatStatement::Accept(Visitor* visitor) const {
	return visitor->VisitRepeat(this);
}

Expression const* RepeatStatement::Count() const {
	return m_Count.get();
}

BlockExpression const* RepeatStatement::Block() const {
	return m_Block.get();
}

ExpressionStatement::ExpressionStatement(unique_ptr<Expression> expr) : m_Expr(move(expr)) {
}

Value ExpressionStatement::Accept(Visitor* visitor) const {
	return m_Expr->Accept(visitor);
}

Expression const* ExpressionStatement::Expr() const {
	return m_Expr.get();
}

Logo2::WhileStatement::WhileStatement(unique_ptr<Expression> condition, unique_ptr<BlockExpression> body) :
	m_Condition(move(condition)), m_Body(move(body)) {
}

Value Logo2::WhileStatement::Accept(Visitor* visitor) const {
	return visitor->VisitWhile(this);
}

Expression const* Logo2::WhileStatement::Condition() const {
	return m_Condition.get();
}

BlockExpression const* Logo2::WhileStatement::Body() const {
	return m_Body.get();
}

Logo2::IfThenElseExpression::IfThenElseExpression(unique_ptr<Expression> condition, unique_ptr<Expression> thenExpr, unique_ptr<Expression> elseExpr) : 
	m_Condition(move(condition)), m_Then(move(thenExpr)), m_Else(move(elseExpr)) {
}

Value Logo2::IfThenElseExpression::Accept(Visitor* visitor) const {
	return visitor->VisitIfThenElse(this);
}

Expression const* Logo2::IfThenElseExpression::Condition() const {
	return m_Condition.get();
}

Expression const* Logo2::IfThenElseExpression::Then() const {
	return m_Then.get();
}

Expression const* Logo2::IfThenElseExpression::Else() const {
	return m_Else.get();
}

Logo2::FunctionDeclaration::FunctionDeclaration(string name, vector<string> parameters, unique_ptr<Expression> body) : 
	m_Name(move(name)), m_Parameters(move(parameters)), m_Body(move(body)) {
}

Value Logo2::FunctionDeclaration::Accept(Visitor* visitor) const {
	return visitor->VisitFunctionDeclaration(this);
}

string const& Logo2::FunctionDeclaration::Name() const {
	return m_Name;
}

vector<string> const& Logo2::FunctionDeclaration::Parameters() const {
	return m_Parameters;
}

Expression const* Logo2::FunctionDeclaration::Body() const {
	return m_Body.get();
}

Logo2::ReturnStatement::ReturnStatement(unique_ptr<Expression> expr) : m_Expr(move(expr)) {
}

Value Logo2::ReturnStatement::Accept(Visitor* visitor) const {
	return visitor->VisitReturn(this);
}

Expression const* Logo2::ReturnStatement::ReturnValue() const {
	return m_Expr.get();
}

Logo2::BreakOrContinueStatement::BreakOrContinueStatement(bool cont) : m_IsContinue(cont) {
}

Value Logo2::BreakOrContinueStatement::Accept(Visitor* visitor) const {
	return visitor->VisitBreakContinue(this);
}

bool Logo2::BreakOrContinueStatement::IsContinue() const {
	return m_IsContinue;
}

Logo2::ForStatement::ForStatement(unique_ptr<Statement> init, unique_ptr<Expression> whileExpr, unique_ptr<Expression> incExpr, unique_ptr<BlockExpression> body) :
	m_Init(move(init)), m_While(move(whileExpr)), m_Inc(move(incExpr)), m_Body(move(body)) {
}

Value Logo2::ForStatement::Accept(Visitor* visitor) const {
	return visitor->VisitFor(this);
}

Statement const* Logo2::ForStatement::Init() const {
	return m_Init.get();
}

Expression const* Logo2::ForStatement::While() const {
	return m_While.get();
}

Expression const* Logo2::ForStatement::Inc() const {
	return m_Inc.get();
}

BlockExpression const* Logo2::ForStatement::Body() const {
	return m_Body.get();
}

Value Statements::Accept(Visitor* visitor) const {
	return visitor->VisitStatements(this);
}

void Statements::Add(unique_ptr<Statement> stmt) {
	m_Stmts.push_back(move(stmt));
}

vector<unique_ptr<Statement>> const& Statements::Get() const {
	return m_Stmts;
}

AnonymousFunctionExpression::AnonymousFunctionExpression(vector<string> args, unique_ptr<Expression> body) :
	m_Args(move(args)), m_Body(move(body)) {
}

Value Logo2::AnonymousFunctionExpression::Accept(Visitor* visitor) const {
	return visitor->VisitAnonymousFunction(this);
}

vector<string> const& Logo2::AnonymousFunctionExpression::Args() const {
	return m_Args;
}

Expression const* Logo2::AnonymousFunctionExpression::Body() const {
	return m_Body.get();
}

bool Logo2::Statement::IsStatement() const {
	return true;
}
