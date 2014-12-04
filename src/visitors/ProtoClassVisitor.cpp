#include <iostream>
#include "ProtoClassVisitor.hpp"


void ProtoClassVisitor::visit(shptr<const ast::ClassDeclaration> classDeclaration)
{
	std::cout << "Visiting class type of " << classDeclaration->getName() << std::endl;
	ir_type* classType = new_type_class(new_id_from_str(classDeclaration->getName().c_str()));
	set_type_alignment_bytes(classType, 8U);
	FirmInterface::getInstance().addClassType(classDeclaration->getIdent(), classType);
}
