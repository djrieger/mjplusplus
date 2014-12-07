#include "PostfixOpsVisitor.hpp"
#include "VariableDeclVisitor.hpp"
#include "../ast/FieldAccess.hpp"

PostfixOpsVisitor::PostfixOpsVisitor(ExpressionVisitor& expressionVisitor): storeValue(NULL), expressionVisitor(expressionVisitor)
{}

void PostfixOpsVisitor::visit(shptr<ast::FieldAccess const> fieldAccess)
{
	std::cout << "Visiting FieldAccess" << std::endl;
	// Member
	VariableDeclVisitor vdVisitor(expressionVisitor.getResultNode(), storeValue);
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

	std::cout << "postfixopsvisitor done" << std::endl;
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

void PostfixOpsVisitor::setStoreValue(ir_node* storeValue)
{
	this->storeValue = storeValue;
}
