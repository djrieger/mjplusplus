#include "MemberVisitor.hpp"

MemberVisitor::MemberVisitor(FirmInterface& firmInterface): FirmVisitor(firmInterface)
{

}

void MemberVisitor::visit(shptr<const ast::Node> node)
{

}

void MemberVisitor::visit(shptr<const ast::MethodDeclaration> node)
{
	function_graph = firmInterface.generateMethod(owner, node);
}

void MemberVisitor::visit(shptr<const ast::FieldDeclaration> fieldDeclaration)
{
	// TODO switch on fieldDeclaration->getType()
	if (fieldDeclaration->getType()->isInteger())
	{

	}
}

ir_graph* MemberVisitor::getGraph() const
{
	return function_graph;
}