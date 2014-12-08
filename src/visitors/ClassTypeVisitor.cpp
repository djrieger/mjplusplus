#include <iostream>
#include "ProtoMemberVisitor.hpp"
#include "ClassTypeVisitor.hpp"


void ClassTypeVisitor::visit(shptr<const ast::ClassDeclaration> classDeclaration)
{
	ir_type* classType = FirmInterface::getInstance()
	                     .getType(std::make_shared<ast::Type>(classDeclaration->getIdent()));

	setOwner(get_pointer_points_to_type(classType));
	ProtoMemberVisitor visitor(*this);

	for (auto& member : *classDeclaration->getMembers())
		member->accept(visitor);
}
