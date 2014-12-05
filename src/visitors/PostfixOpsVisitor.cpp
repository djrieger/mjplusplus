#include "PostfixOpsVisitor.hpp"
#include "VariableDeclVisitor.hpp"
#include "../ast/FieldAccess.hpp"

PostfixOpsVisitor::PostfixOpsVisitor(ExpressionVisitor& expressionVisitor): expressionVisitor(expressionVisitor)
{}

void PostfixOpsVisitor::visit(shptr<ast::FieldAccess const> fieldAccess)
{
	std::cout << "Visiting FieldAccess" << std::endl;
	// Member
	VariableDeclVisitor vdVisitor(resultNode);
	auto decl = fieldAccess->getDeclaration();

	if (decl)
	{
		std::cout << "got declaration of field" << std::endl;
		decl->accept(vdVisitor);
		resultNode = vdVisitor.getResultNode();
		resultType = vdVisitor.getResultType();
	}
	else
		std::cout << "got System in FieldAccess (currently nothing is done with it)" << std::endl;
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
