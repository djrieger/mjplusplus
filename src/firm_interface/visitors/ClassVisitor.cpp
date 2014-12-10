#include <iostream>
#include "ClassVisitor.hpp"
#include "MemberVisitor.hpp"

void firm::visitor::ClassVisitor::visit(shptr<const ast::ClassDeclaration> classDeclaration)
{
	ir_type* classType = FirmInterface::getInstance()
	                     .getType(std::make_shared<ast::Type>(classDeclaration->getIdent()));

	setOwner(get_pointer_points_to_type(classType));
	MemberVisitor visitor(*this);

	for (auto& member : *classDeclaration->getMembers())
		member->accept(visitor);
}
