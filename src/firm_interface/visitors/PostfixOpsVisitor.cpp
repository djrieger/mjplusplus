#include "PostfixOpsVisitor.hpp"
#include "VariableDeclVisitor.hpp"
#include "../../ast/FieldAccess.hpp"
#include "../../ast/ArrayAccess.hpp"

namespace firm
{
	namespace visitor
	{
		PostfixOpsVisitor::PostfixOpsVisitor(ExpressionVisitor& expressionVisitor): expressionVisitor(expressionVisitor)
		{
		}

		void PostfixOpsVisitor::visit(shptr<ast::po::FieldAccess const> fieldAccess)
		{
			// Member
			ir_node* current_this = expressionVisitor.getResultNode();
			VariableDeclVisitor vdVisitor(current_this);
			vdVisitor.setDoStore(doStore);
			auto decl = fieldAccess->getDeclaration();

			if (decl)
			{
				decl->accept(vdVisitor);
				resultNode = vdVisitor.getResultNode();
				resultType = vdVisitor.getResultType();
				varNum = vdVisitor.getVarNum();
			}

			// else: got System in FieldAccess
		}

		void PostfixOpsVisitor::visit(shptr<ast::po::ArrayAccess const> arrayAccess)
		{
			// Get the node for the array we are accessing.
			ir_node* arrayAddress = expressionVisitor.getResultNode();
			ir_type* arrayType = expressionVisitor.getResultType();
			ir_type* elementType = get_pointer_points_to_type(arrayType);

			// Build an ExpressionVisitor to find out the access offset (which is an
			// expression).
			ExpressionVisitor accessOffsetVisitor;
			arrayAccess->getAccessOffset()->accept(accessOffsetVisitor);
			ir_node* accessOffset = accessOffsetVisitor.getResultNode();

			/* replaced arrays with pointers since get_type_mode doesn't like them; as result Sel is not usable... */
			//ir_node* elementAddress = new_Sel(arrayAddress, accessOffset, arrayType);

			ir_mode* addr_mode = get_reference_mode_unsigned_eq(mode_P);
			ir_node* offset_node = new_Const_long(addr_mode, get_type_size_bytes(elementType));
			offset_node = new_Mul(offset_node, new_Conv(accessOffset, addr_mode), addr_mode);
			ir_node* elementAddress = new_Conv(new_Add(new_Conv(arrayAddress, addr_mode), offset_node, addr_mode), mode_P);

			if (doStore)
			{
				resultNode = elementAddress;
				resultType = elementType;
			}
			else
			{
				// We are retrieving an array element.
				ir_node* load = new_Load(get_store(), elementAddress, get_type_mode(elementType), elementType, cons_none);
				set_store(new_Proj(load, mode_M, pn_Load_M));
				resultNode = new_Proj(load, get_type_mode(elementType), pn_Load_res);
				resultType = elementType;
			}
		}


		void PostfixOpsVisitor::visit(shptr<ast::po::MethodInvocation const> methodInvocation)
		{
			std::string mangledMethodName = methodInvocation->getDeclaration()->mangle();
			ir_node* caller;

			if (mangledMethodName == "_COut_Mprintln")
			{
				//we got out System.out.println
				caller = FirmInterface::getInstance().createNullPointerNode();
			}

			else
				caller = expressionVisitor.getResultNode();

			std::tie(resultNode, resultType) =
			    FirmInterface::getInstance().createNodeForMethodCall(caller, methodInvocation);
		}
	}
}
