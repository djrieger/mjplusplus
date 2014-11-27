#include "MemberVisitor.hpp"

MemberVisitor::MemberVisitor(ClassVisitor& classVisitor): classVisitor(classVisitor)
{
	setOwner(classVisitor.getOwner());
}

void MemberVisitor::visit(shptr<const ast::Node> node)
{

}

ir_graph* MemberVisitor::visit(shptr<const ast::MethodDeclaration> methodDeclaration)
{
	// generate method:

	unsigned int paramsCount = methodDeclaration->getParameters()->size();
	bool hasReturnType = !methodDeclaration->getReturnType()->isVoid();
	ir_type* methodType = new_type_method(paramsCount, hasReturnType);

	int i = 0;

	for (auto& param : *methodDeclaration->getParameters())
	{
		set_method_param_type(methodType, i, new_type_primitive(mode_Is)); // TODO: Implement ref and bool types
		i++;
	}

	if (hasReturnType)
	{
		set_method_res_type(methodType, 0, new_type_primitive(mode_Is)); // TODO: Implement ref and bool types
	}

	ir_entity* ent = new_entity(owner, new_id_from_str(methodDeclaration->getName().c_str()), methodType);
	return new_ir_graph(ent, methodDeclaration->countVariableDeclarations());
}

ir_entity* MemberVisitor::visit(shptr<const ast::FieldDeclaration> fieldDeclaration)
{
	// TODO switch on fieldDeclaration->getType()
	ir_type* field_ir_type;
	auto fieldASTType = fieldDeclaration->getType();

	if (fieldASTType->isInteger())
		field_ir_type = new_type_primitive(mode_Is);

	/*	else if (fieldASTType->isArray())
		{
			if (fieldASTType->getPrimitiveType()->isInteger())
				field_ir_type = new_type_array(new_type_primitive(mode_Is));
			else
			{

			}
		}*/

	new_entity(owner, new_id_from_str(fieldDeclaration->getName().c_str()), field_ir_type);
}

ir_graph* MemberVisitor::getGraph() const
{
	return function_graph;
}