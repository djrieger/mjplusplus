#include <iostream>
#include "ProtoMemberVisitor.hpp"
#include "ClassTypeVisitor.hpp"


void ClassTypeVisitor::visit(shptr<const ast::ClassDeclaration> classDeclaration)
{
	std::cout << "Visiting class type of " << classDeclaration->getName() << std::endl;
	ir_type* classType = FirmInterface::getInstance()
	                     .getType(std::make_shared<ast::Type>(classDeclaration->getIdent()));

	setOwner(classType);
	ProtoMemberVisitor visitor(*this);

	for (auto& member : *classDeclaration->getMembers())
	{
		member->accept(visitor);
		// TODO: Do something with result
		//visitor.getResult();
	}
}
