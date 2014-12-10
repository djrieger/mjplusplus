#include <iostream>

#include "ProtoMemberVisitor.hpp"
#include "StatementVisitor.hpp"

namespace firm
{
	namespace visitor
	{
		ProtoMemberVisitor::ProtoMemberVisitor(ProtoClassVisitor& classVisitor): byte_pos(-1), int_pos(-1), classVisitor(classVisitor)
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
			// expand class for this member
			auto fieldASTType = fieldDeclaration->getType();
			ir_type* field_ir_type = FirmInterface::getInstance().getType(fieldASTType);

			auto offset = get_type_size_bytes(owner);
			// object layout debug print
			//std::cout << "old: " << offset << "[" << byte_pos << ',' << int_pos << "], adding " << fieldDeclaration->mangle() << std::endl;

			bool need_space = true;

			if (get_type_size_bytes(field_ir_type) == 1)
			{
				/* boolean */
				if (byte_pos > -1)
				{
					need_space = false;
					offset = byte_pos;
					byte_pos++;

					if (byte_pos % 4 == 0)
						byte_pos = -1;
				}
				else if (int_pos > -1)
				{
					need_space = false;
					offset = int_pos;
					byte_pos = int_pos + 1;
					int_pos = -1;
				}
				else
				{
					byte_pos = offset + 1;
					int_pos = offset + 4;
				}
			}
			else if (get_type_size_bytes(field_ir_type) == 4)
			{
				/* int */
				if (int_pos > -1)
				{
					need_space = false;
					offset = int_pos;
					int_pos = -1;
				}
				else
					int_pos = offset + 4;
			}

			if (need_space)
				set_type_size_bytes(owner, offset + 8U);

			ir_entity* field = new_entity(owner,
			                              new_id_from_str(fieldDeclaration->mangle().c_str()),
			                              field_ir_type);
			FirmInterface::getInstance().addField(owner, fieldDeclaration->mangle(), field);
			set_entity_offset(field, offset);
			// object layout debug print
			//std::cout << "new: " << get_type_size_bytes(owner) << "[" << byte_pos << ',' << int_pos << "] put at " << offset << std::endl;
		}

		ir_graph* ProtoMemberVisitor::getGraph() const
		{
			return function_graph;
		}
	}
}
