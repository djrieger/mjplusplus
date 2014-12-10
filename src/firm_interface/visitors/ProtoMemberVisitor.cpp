#include "ProtoMemberVisitor.hpp"
#include "StatementVisitor.hpp"

namespace firm
{
	namespace visitor
	{
		ProtoMemberVisitor::ProtoMemberVisitor(ProtoClassVisitor& classVisitor): classVisitor(classVisitor)
		{
			setOwner(classVisitor.getOwner());
		}

		void ProtoMemberVisitor::visit(shptr<const ast::MethodDeclaration> methodDeclaration)
		{
			std::string mangledMethodName = methodDeclaration->mangle();
			ir_entity* ent = FirmInterface::getInstance().createMethodEntity(owner, methodDeclaration);
			FirmInterface::getInstance().addMethod(owner, mangledMethodName, ent);
		}

		void ProtoMemberVisitor::visit(shptr<const ast::MainMethodDeclaration>)
		{
			;
		}

		void ProtoMemberVisitor::visit(shptr<const ast::FieldDeclaration> fieldDeclaration)
		{
			auto fieldASTType = fieldDeclaration->getType();
			ir_type* field_ir_type = FirmInterface::getInstance().getType(fieldASTType);
			// expand class for this member. TODO: not everything needs 8 bytes, try to pack things
			auto offset = get_type_size_bytes(owner);

			set_type_size_bytes(owner, offset + 8U);
			ir_entity* field = new_entity(owner,
			                              new_id_from_str(fieldDeclaration->mangle().c_str()),
			                              field_ir_type);
			FirmInterface::getInstance().addField(owner, fieldDeclaration->mangle(), field);
			set_entity_offset(field, offset);
		}

		ir_graph* ProtoMemberVisitor::getGraph() const
		{
			return function_graph;
		}
	}
}
