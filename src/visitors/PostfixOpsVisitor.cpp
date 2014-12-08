#include "PostfixOpsVisitor.hpp"
#include "VariableDeclVisitor.hpp"
#include "../ast/FieldAccess.hpp"
#include "../ast/ArrayAccess.hpp"
#include <cassert>

PostfixOpsVisitor::PostfixOpsVisitor(ExpressionVisitor& expressionVisitor): storeValue(NULL), expressionVisitor(expressionVisitor)
{
}

void PostfixOpsVisitor::visit(shptr<ast::FieldAccess const> fieldAccess)
{
	std::cout << "Visiting FieldAccess" << std::endl;
	// Member
	ir_node* current_this = expressionVisitor.getResultNode();
	std::cout << "Field access on current this: " << current_this << std::endl;
	VariableDeclVisitor vdVisitor(current_this, storeValue);
	auto decl = fieldAccess->getDeclaration();

	if (decl)
	{
		std::cout << "got declaration of field" << std::endl;
		decl->accept(vdVisitor);
		resultNode = vdVisitor.getResultNode();
		resultType = vdVisitor.getResultType();
	}
	else
		std::cout << "got System in FieldAccess (currently nothing is done with it)" << std::endl;

	std::cout << "postfixopsvisitor done" << std::endl;
}

void PostfixOpsVisitor::visit(shptr<ast::ArrayAccess const> arrayAccess)
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

	ir_node* mem = get_store();

	if (storeValue)
	{
		// We are setting an array element.
		ir_node* store = new_Store(mem, elementAddress, storeValue, elementType, cons_none);
		set_store(new_Proj(store, mode_M, pn_Store_M));
		resultNode = storeValue;
	}
	else
	{
		// We are retrieving an array element.
		ir_node* load = new_Load(mem, elementAddress, get_type_mode(elementType), elementType, cons_none);
		set_store(new_Proj(load, mode_M, pn_Load_M));
		resultNode = new_Proj(load, get_type_mode(elementType), pn_Load_res);
	}

	resultType = elementType;
}


void PostfixOpsVisitor::visit(shptr<ast::MethodInvocation const> methodInvocation)
{
	std::cout << "Visiting MethodInvocation" << std::endl;

	ir_node* caller = expressionVisitor.getResultNode();
	this->resultNode = FirmInterface::getInstance().createNodeForMethodCall(caller, methodInvocation);
}

void PostfixOpsVisitor::setStoreValue(ir_node* storeValue)
{
	this->storeValue = storeValue;
}
