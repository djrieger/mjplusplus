#include "ClassVisitor.hpp"
#include "MemberVisitor.hpp"

ClassVisitor::ClassVisitor() {}

void ClassVisitor::visit(shptr<const ast::ClassDeclaration> classDeclaration)
{
	ir_type* classType = new_type_class(new_id_from_str(classDeclaration->getName().c_str()));
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