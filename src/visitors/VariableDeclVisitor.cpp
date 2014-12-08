#include "VariableDeclVisitor.hpp"

VariableDeclVisitor::VariableDeclVisitor(ir_node* current_this, ir_node* store_value): current_this(current_this), store_value(store_value)
{
}

void VariableDeclVisitor::visit(shptr<ast::FieldDeclaration const> fieldDeclaration)
{
	if (!current_this)
		current_this = get_value(0, mode_P);

	// Our variable was declared as field => Our variable is a fieldaccess.
	ir_node* mem = get_store();
	ir_type* this_type = FirmInterface::getInstance().getType(std::make_shared<ast::Type>(fieldDeclaration->getDeclaration()->getIdent()));
	ir_entity* field = FirmInterface::getInstance().getFieldEntity(get_pointer_points_to_type(this_type), fieldDeclaration->mangle());
	ir_mode* addr_mode = get_reference_mode_unsigned_eq(mode_P);
	ir_node* offset_node = new_Const_long(addr_mode, get_entity_offset(field));
	ir_node* addr = new_Conv(new_Add(new_Conv(current_this, addr_mode), offset_node, addr_mode), mode_P);
	ir_type* field_type = get_entity_type(field);

	if (store_value)
	{
		ir_node* store = new_Store(mem, addr, store_value, field_type, cons_none);
		set_store(new_Proj(store, mode_M, pn_Store_M));
		resultNode = store_value;
	}
	else
	{
		ir_node* load = new_Load(mem, addr, get_type_mode(field_type), field_type, cons_none);
		set_store(new_Proj(load, mode_M, pn_Load_M));
		resultNode = new_Proj(load, get_type_mode(field_type), pn_Load_res);
	}

	resultType = field_type;
}

void VariableDeclVisitor::visit(shptr<ast::LVDStatement const> lvdStatement)
{
	//Our variable was declared as LV => Our variable is a LV.
	visitLVDStatementOrTypeIdent(lvdStatement, lvdStatement->getIdent()->getName());

	ir_printf("%F, type %F, node %F\n", FirmInterface::getInstance().getMode(lvdStatement->getDeclType()), resultType, resultNode);
}

void VariableDeclVisitor::visit(shptr<ast::TypeIdent const> typeIdent)
{
	// Our variable was declared as type ident => Our variable is a parameter (and we treat it as LV).
	visitLVDStatementOrTypeIdent(typeIdent, typeIdent->getName());
}

void VariableDeclVisitor::visitLVDStatementOrTypeIdent(shptr<ast::VariableDeclaration const> variableDeclaration, std::string varName)
{
	auto varMap = FirmInterface::getInstance().getVarMap();

	int pos = (*varMap)[varName];
	resultType = FirmInterface::getInstance().getType(variableDeclaration->getDeclType());

	if (store_value)
	{
		set_value(pos, store_value);
		resultNode = store_value;
	}
	else
		resultNode = get_value(pos, FirmInterface::getInstance().getMode(variableDeclaration->getDeclType()));
}
