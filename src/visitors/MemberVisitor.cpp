#include "MemberVisitor.hpp"

MemberVisitor::MemberVisitor(ClassVisitor& classVisitor): classVisitor(classVisitor)
{
	setOwner(classVisitor.getOwner());
}

void MemberVisitor::createReturnNodeAndFinalize(ir_graph* irg)
{
	set_current_ir_graph(irg);

	ir_node* currentMemState = get_store();
	ir_node* x = new_Return(currentMemState, 0, NULL);
	add_immBlock_pred(get_irg_end_block(irg), x);

	irg_finalize_cons(irg);
}

void MemberVisitor::visit(shptr<const ast::MethodDeclaration> methodDeclaration)
{
	unsigned int paramsCount = methodDeclaration->getParameters()->size();
	bool hasReturnType = !methodDeclaration->getReturnType()->isVoid();
	ir_type* methodType = new_type_method(paramsCount + 1, hasReturnType);

	// this pointer as first parameter
	set_method_param_type(methodType, 0, owner);

	int i = 1;

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

	ir_entity* ent = new_entity(owner, new_id_from_str(methodDeclaration->mangle(methodDeclaration->getDeclaration()->getName()).c_str()), methodType);
	FirmInterface::getInstance().addMethod(owner, methodDeclaration->getName(), ent);
	//TODO: SimpleIf example includes parameters into local variable count
	function_graph = new_ir_graph(ent, methodDeclaration->countVariableDeclarations());

	createReturnNodeAndFinalize(function_graph);
}

void MemberVisitor::visit(shptr<const ast::MainMethodDeclaration> mainMethodDecl)
{
#ifndef __APPLE__
	std::string mainMethodName = "main";
#else
	std::string mainMethodName = "_main";
#endif

	ir_type* proc_main = new_type_method(0, 0);
	ir_type* globalOwner = get_glob_type();
	ir_entity* mainMethodEntity = new_entity(globalOwner, new_id_from_str(mainMethodName.c_str()), proc_main);
	ir_graph* irg = new_ir_graph(mainMethodEntity, mainMethodDecl->countVariableDeclarations());

	createReturnNodeAndFinalize(irg);
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
	FirmInterface::getInstance().addField(owner, fieldDeclaration->getName(), field);
	set_entity_offset(field, offset);
}

ir_graph* MemberVisitor::getGraph() const
{
	return function_graph;
}
