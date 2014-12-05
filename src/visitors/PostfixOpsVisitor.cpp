#include "PostfixOpsVisitor.hpp"
#include "../ast/FieldAccess.hpp"
#include "../visitors/VariableDeclVisitor.hpp"

PostfixOpsVisitor::PostfixOpsVisitor(ExpressionVisitor& expressionVisitor): expressionVisitor(expressionVisitor)
{}

void PostfixOpsVisitor::visit(shptr<ast::FieldAccess const> fieldAccess)
{
	std::cout << "Visiting FieldAccess" << std::endl;
	//TODO: use VariableDeclVisitor
	VariableDeclVisitor vdVisitor(expressionVisitor.getResultNode(), fieldAccess);
	auto decl = fieldAccess->getDeclaration();

	std::cout << "got declaration " << std::endl;
	decl->accept(vdVisitor);
	resultNode = vdVisitor.getResultNode();
	resultType = vdVisitor.getResultType();
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

ir_type* PostfixOpsVisitor::getResultType() const
{
	return this->resultType;
}
