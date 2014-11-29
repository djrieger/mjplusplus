#include "MemberVisitor.hpp"

MemberVisitor::MemberVisitor(ClassVisitor& classVisitor): classVisitor(classVisitor)
{
	setOwner(classVisitor.getOwner());
}

void MemberVisitor::visit(shptr<const ast::MethodDeclaration> methodDeclaration)
{
	// generate method:

	unsigned int paramsCount = methodDeclaration->getParameters()->size();
	bool hasReturnType = !methodDeclaration->getReturnType()->isVoid();
	ir_type* methodType = new_type_method(paramsCount, hasReturnType);

	int i = 0;

	for (auto& param : *methodDeclaration->getParameters())
	{
		auto type = FirmInterface::getInstance().getType(param->getType());
		set_method_param_type(methodType, i, type);
		i++;
	}

	if (hasReturnType)
	{
		auto type = FirmInterface::getInstance().getType(methodDeclaration->getReturnType());
		set_method_res_type(methodType, 0, type);
	}

	ir_entity* ent = new_entity(owner, new_id_from_str(methodDeclaration->getName().c_str()), methodType);
	//TODO: SimpleIf example includes parameters into local variable count
	function_graph = new_ir_graph(ent, methodDeclaration->countVariableDeclarations());
}

void MemberVisitor::visit(shptr<const ast::FieldDeclaration> fieldDeclaration)
{
	// TODO switch on fieldDeclaration->getType()
	auto fieldASTType = fieldDeclaration->getType();
	ir_type* field_ir_type = FirmInterface::getInstance().getType(fieldASTType);
	// expand class for this member. TODO: not everything needs 8 bytes, try to pack things
	auto offset = get_type_size_bytes(owner);
	set_type_size_bytes(owner, offset + 8U);
	ir_entity* field = new_entity(owner, new_id_from_str(fieldDeclaration->getName().c_str()), field_ir_type);
	set_entity_offset(field, offset);
}

ir_graph* MemberVisitor::getGraph() const
{
	return function_graph;
}
