#include <iostream>

#include "ClassVisitor.hpp"
#include "MemberVisitor.hpp"

void ClassVisitor::visit(shptr<const ast::ClassDeclaration> classDeclaration)
{
	std::cout << "Visiting class " << classDeclaration->getName() << std::endl;
	ir_type* classType = new_type_class(new_id_from_str(classDeclaration->getName().c_str()));
	set_type_alignment_bytes(classType, 8U);
	FirmInterface::getInstance().addClassType(classDeclaration->getIdent(), classType);
	setOwner(classType);
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
