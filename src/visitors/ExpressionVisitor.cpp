#include "ExpressionVisitor.hpp"

ir_node* ExpressionVisitor::getResultNode() const
{
	return resultNode;
}

void ExpressionVisitor::visit(shptr<ast::be::Plus const> plusExpr)
{
	plusExpr->getLeftChild()->accept(*this);
	ir_node* left = this->resultNode;

	plusExpr->getRightChild()->accept(*this);
	ir_node* right = this->resultNode;

	this->resultNode = FirmInterface::getInstance().createOperation(plusExpr, left, right);
}

// primary expressions
void ExpressionVisitor::visit(shptr<ast::pe::Bool const> boolExpr)
{
	bool value = boolExpr->getValue();
	this->resultNode = FirmInterface::getInstance().createNodeForBoolConstant(value);
}
void ExpressionVisitor::visit(shptr<ast::pe::Ident const> identExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::pe::Integer const> integerExpr)
{
	int x = atoi(integerExpr->getStringValue().c_str());
	this->resultNode = FirmInterface::getInstance().createNodeForIntegerConstant(x);
}
void ExpressionVisitor::visit(shptr<ast::pe::MethodInvocation const> methodInvocationExpr)
{
	this->resultNode = FirmInterface::getInstance().createNodeForMethodCall(methodInvocationExpr);
}
void ExpressionVisitor::visit(shptr<ast::pe::NewArrayExpression const> newArrayExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::pe::NewObjectExpression const> newObjectExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::pe::Object const> objectExpr)
{
	;
}

// unary expressions
void ExpressionVisitor::visit(shptr<ast::ue::Neg const> negExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::ue::Not const> notExpr)
{
	;
}

// binary expressions
void ExpressionVisitor::visit(shptr<ast::be::AndAnd const> andAndExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::be::Eq const> eqExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::be::EqEq const> eqEqExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::be::GreaterThan const> greaterThanExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::be::GreaterThanEq const> greaterThanEqExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::be::Invalid const> invalidExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::be::LessThan const> lessThanExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::be::LessThanEq const> lessThanEqExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::be::Minus const> minusExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::be::Mod const> modExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::be::Mult const> multExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::be::NotEq const> notEqExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::be::OrOr const> orOrExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::be::Slash const> slashExpr)
{
	;
}

// postfix expression
void ExpressionVisitor::visit(shptr<ast::PostfixExpression const> postfixExpression)
{
	;
}
