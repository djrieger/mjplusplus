#include "BoolExpressionVisitor.hpp"
#include "ExpressionVisitor.hpp"
#include <sstream>
#include <algorithm>

BoolExpressionVisitor::BoolExpressionVisitor(ir_node* thenBlock, ir_node* elseBlock): thenBlock(thenBlock), elseBlock(elseBlock)
{}

void BoolExpressionVisitor::boolFromExpression(shptr<ast::Expression const> expression)
{
	std::cout << "boolFromExpression: then = " << thenBlock << ", else = " << elseBlock << std::endl;

	ExpressionVisitor exprVisit;
	expression->accept(exprVisit);

	ir_node* cmpNode = new_Cmp(FirmInterface::getInstance().createNodeForBooleanConstant(true), exprVisit.getResultNode(), ir_relation::ir_relation_equal);
	ir_node* condNode = new_Cond(cmpNode);
	ir_node* projTrue = new_Proj(condNode, get_modeX(), pn_Cond_true);
	ir_node* projFalse = new_Proj(condNode, get_modeX(), pn_Cond_false);
	add_immBlock_pred(thenBlock, projTrue);
	add_immBlock_pred(elseBlock, projFalse);
}

void BoolExpressionVisitor::visitRelationalExpression(shptr<ast::be::BinaryExpression const> binaryExpression, ir_relation relation)
{
	std::cout << "visitRelationalExpression" << std::endl;

	ExpressionVisitor exprVisit;
	binaryExpression->getLeftChild()->accept(exprVisit);
	ir_node* left = exprVisit.getResultNode();
	binaryExpression->getRightChild()->accept(exprVisit);
	ir_node* right = exprVisit.getResultNode();

	ir_node* cmpNode = new_Cmp(left, right, relation);
	ir_node* condNode = new_Cond(cmpNode);
	ir_node* projTrue = new_Proj(condNode, get_modeX(), pn_Cond_true);
	ir_node* projFalse = new_Proj(condNode, get_modeX(), pn_Cond_false);
	add_immBlock_pred(thenBlock, projTrue);
	add_immBlock_pred(elseBlock, projFalse);
}

// primary expressions
void BoolExpressionVisitor::visit(shptr<ast::pe::Bool const> boolExpr)
{
	boolFromExpression(boolExpr);
}

void BoolExpressionVisitor::visit(shptr<ast::pe::Ident const> identExpr)
{
	boolFromExpression(identExpr);
}

void BoolExpressionVisitor::visit(shptr<ast::pe::MethodInvocation const> methodInvocationExpr)
{
	boolFromExpression(methodInvocationExpr);
}

// unary expressions
void BoolExpressionVisitor::visit(shptr<ast::ue::Not const> notExpr)
{
	shptr<ast::Expression> child = notExpr->getChild();

	std::cout << "Before swapping for Not: then = " << thenBlock << ", else = " << elseBlock << std::endl;

	if (notExpr->getSize() & 1)
		std::swap(thenBlock, elseBlock);

	std::cout << "After swapping for Not: then = " << thenBlock << ", else = " << elseBlock << std::endl;
	child->accept(*this);
}

// binary expressions
void BoolExpressionVisitor::visit(shptr<ast::be::Eq const> eqExpr)
{
	boolFromExpression(eqExpr);
}

void BoolExpressionVisitor::visit(shptr<ast::be::AndAnd const> andAndExpr)
{
	ir_node* originalThenBlock = thenBlock;
	ir_node* originalElseBlock = elseBlock;

	ir_node* rightExprBlock = new_immBlock();
	thenBlock = rightExprBlock;
	andAndExpr->getLeftChild()->accept(*this);
	mature_immBlock(rightExprBlock);

	thenBlock = originalThenBlock;
	elseBlock = originalElseBlock;

	set_cur_block(rightExprBlock);

	andAndExpr->getRightChild()->accept(*this);
}

void BoolExpressionVisitor::visit(shptr<ast::be::OrOr const> orOrExpr)
{
	ir_node* originalThenBlock = thenBlock;
	ir_node* originalElseBlock = elseBlock;

	ir_node* rightExprBlock = new_immBlock();
	elseBlock = rightExprBlock; // only change
	orOrExpr->getLeftChild()->accept(*this);
	mature_immBlock(rightExprBlock);

	thenBlock = originalThenBlock;
	elseBlock = originalElseBlock;

	set_cur_block(rightExprBlock);

	orOrExpr->getRightChild()->accept(*this);
}

void BoolExpressionVisitor::visit(shptr<ast::be::EqEq const> eqEqExpr)
{
	std::cout << "visiting EqEq" << std::endl;
	visitRelationalExpression(eqEqExpr, ir_relation::ir_relation_equal);
}

void BoolExpressionVisitor::visit(shptr<ast::be::NotEq const> notEqExpr)
{
	//see documentation for libFirm: ir_releation_less_greater for integers is "not equal".
	visitRelationalExpression(notEqExpr, ir_relation::ir_relation_unordered_less_greater);
}

void BoolExpressionVisitor::visit(shptr<ast::be::GreaterThan const> greaterThanExpr)
{
	visitRelationalExpression(greaterThanExpr, ir_relation::ir_relation_greater);
}

void BoolExpressionVisitor::visit(shptr<ast::be::GreaterThanEq const> greaterThanEqExpr)
{
	visitRelationalExpression(greaterThanEqExpr, ir_relation::ir_relation_greater_equal);
}

void BoolExpressionVisitor::visit(shptr<ast::be::LessThan const> lessThanExpr)
{
	visitRelationalExpression(lessThanExpr, ir_relation::ir_relation_less);
}

void BoolExpressionVisitor::visit(shptr<ast::be::LessThanEq const> lessThanEqExpr)
{
	visitRelationalExpression(lessThanEqExpr, ir_relation::ir_relation_less_equal);
}

void BoolExpressionVisitor::visit(shptr<ast::be::Invalid const>)
{
	std::cerr << "ExpressionVisitor visited Invalid: in" << __FILE__
	          << " at " << __LINE__ << std::endl;
}

// postfix expression
void BoolExpressionVisitor::visit(shptr<ast::PostfixExpression const> postfixExpression)
{
	boolFromExpression(postfixExpression);
}
