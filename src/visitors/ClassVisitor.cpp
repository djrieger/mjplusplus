#include <iostream>

#include "ClassVisitor.hpp"
#include "MemberVisitor.hpp"

void ClassVisitor::visit(shptr<const ast::ClassDeclaration> classDeclaration)
{
	std::cout << "Visiting class " << classDeclaration->getName() << std::endl;
	ir_type* classType = FirmInterface::getInstance()
	                     .getType(std::make_shared<ast::Type>(classDeclaration->getIdent()));

	setOwner(get_pointer_points_to_type(classType));
	MemberVisitor visitor(*this);

	for (auto& member : *classDeclaration->getMembers())
	{
		member->accept(visitor);
		// TODO: Do something with result
		//visitor.getResult();
	}

	// new_type_size_bytes(irtype, ...)
	// SEE get_ir_type()

}
