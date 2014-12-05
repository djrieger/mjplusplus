#include "PostfixOpsVisitor.hpp"

PostfixOpsVisitor::PostfixOpsVisitor(ExpressionVisitor& expressionVisitor): expressionVisitor(expressionVisitor)
{}

void PostfixOpsVisitor::visit(shptr<ast::FieldAccess const> fieldAccess)
{
	std::cout << "Visiting FieldAccess" << std::endl;
	//TODO: use VariableDeclVisitor
}

void PostfixOpsVisitor::visit(shptr<ast::ArrayAccess const> arrayAccess)
{

}

void PostfixOpsVisitor::visit(shptr<ast::MethodInvocation const> methodInvocation)
{
	std::cout << "Visiting MethodInvocation" << std::endl;

	ir_node* caller = expressionVisitor.getResultNode();
	this->resultNode = FirmInterface::getInstance().createNodeForMethodCall(caller, methodInvocation);
}


ir_node* PostfixOpsVisitor::getResultNode() const
{
	return this->resultNode;
}