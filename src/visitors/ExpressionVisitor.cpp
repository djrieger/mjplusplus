#include "ExpressionVisitor.hpp"

void visit(shptr<ast::be::Plus const> plusExpr)
{
	plusExpr->getLeftChild()->accept(this);
	ir_node* left = this->resultNode;

	plusExpr->getRightChild()->accept(this);
	ir_node* right = this->resultNode;

	this->resultNode = FirmInterface::getInstance->createOperation(plusExpr, left, right);
}