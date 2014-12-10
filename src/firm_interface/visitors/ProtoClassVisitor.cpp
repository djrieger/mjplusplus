#include <iostream>
#include "ProtoClassVisitor.hpp"


void firm::visitor::ProtoClassVisitor::visit(shptr<const ast::ClassDeclaration> classDeclaration)
{
	ir_type* classType = new_type_class(new_id_from_str(classDeclaration->getName().c_str()));
	set_type_alignment_bytes(classType, 8U);
	FirmInterface::getInstance().addClassType(classDeclaration->getIdent(), classType);
}