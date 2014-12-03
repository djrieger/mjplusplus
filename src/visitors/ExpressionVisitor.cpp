#include "ExpressionVisitor.hpp"

ir_node* ExpressionVisitor::getResultNode() const
{
	return resultNode;
}

void ExpressionVisitor::visitBinaryExpression(
	shptr<ast::be::BinaryExpression const> binExpr,
	std::function<ir_node* (ir_node*, ir_node*)> createResultNode)
{
	binExpr->getLeftChild()->accept(*this);
	ir_node* left = this->resultNode;

	binExpr->getRightChild()->accept(*this);
	ir_node* right = this->resultNode;

	this->resultNode = createResultNode(left, right);
}

void ExpressionVisitor::visitRelationalExpression(shptr<ast::be::BinaryExpression const> binaryExpression, ir_relation relation) {
	visitBinaryExpression(binaryExpression, [binaryExpression, relation] (ir_node* left, ir_node *right) -> ir_node * {
		return new_d_Cmp(NULL, left, right, relation);
	});
}

void ExpressionVisitor::visit(shptr<ast::be::Plus const> plusExpr)
{
	visitBinaryExpression(plusExpr, [plusExpr] (ir_node *left, ir_node *right) -> ir_node* {
		return FirmInterface::getInstance().createOperation(plusExpr, left, right);
	});	
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
	visitRelationalExpression(eqEqExpr, ir_relation::ir_relation_equal);
}
void ExpressionVisitor::visit(shptr<ast::be::GreaterThan const> greaterThanExpr)
{
	visitRelationalExpression(greaterThanExpr, ir_relation::ir_relation_greater);
}
void ExpressionVisitor::visit(shptr<ast::be::GreaterThanEq const> greaterThanEqExpr)
{
	visitRelationalExpression(greaterThanEqExpr, ir_relation::ir_relation_greater_equal);
}
void ExpressionVisitor::visit(shptr<ast::be::Invalid const> invalidExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::be::LessThan const> lessThanExpr)
{
	visitRelationalExpression(lessThanExpr, ir_relation::ir_relation_less);
}
void ExpressionVisitor::visit(shptr<ast::be::LessThanEq const> lessThanEqExpr)
{
	visitRelationalExpression(lessThanEqExpr, ir_relation::ir_relation_less_equal);
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
