#include "VariableDeclVisitor.hpp"

VariableDeclVisitor::VariableDeclVisitor(ir_node* current_this, shptr<ast::pe::Ident const> ident): current_this(current_this), ident(ident)
{}

void VariableDeclVisitor::visit(shptr<ast::FieldDeclaration const> fieldDeclaration)
{

	std::cout << "vd visit FieldDecl" << std::endl;
	//Our variable was declared as field => Our variable is a fieldaccess.
	ir_node* mem = get_store();
	ir_type* this_type = FirmInterface::getInstance().getType(std::make_shared<ast::Type>(fieldDeclaration->getDeclaration()->getIdent()));
	ir_entity* field = FirmInterface::getInstance().getFieldEntity(get_pointer_points_to_type(this_type), fieldDeclaration->mangle());
	ir_node* addr = new_Add(current_this, FirmInterface::getInstance().createNodeForIntegerConstant(get_entity_offset(field)), mode_P);
	ir_type* field_type = get_entity_type(field);
	ir_node* load = new_Load(mem, addr, get_type_mode(field_type), field_type, cons_none);
	set_store(new_Proj(load, mode_M, pn_Load_M));
	resultNode = new_Proj(load, get_type_mode(field_type), pn_Load_res);
	resultType = field_type;
}

void VariableDeclVisitor::visit(shptr<ast::LVDStatement const> lvdStatement)
{
	//Our variable was declared as LV => Our variable is a LV.
	auto varMap = FirmInterface::getInstance().getVarMap();
	auto varName = lvdStatement->getIdent()->getName();
	int pos = (*varMap)[varName];
	resultType = FirmInterface::getInstance().getType(lvdStatement->getDeclType());
	resultNode = get_value(pos, FirmInterface::getInstance().getMode(lvdStatement->getDeclType()));
	std::cout << "LVDStatement " << varName << "=" << resultNode << "@" << pos << std::endl;
}

void VariableDeclVisitor::visit(shptr<ast::TypeIdent const> typeIdent)
{
	//Our variable was declared as type ident => Our variable is a parameter.
}

ir_node* VariableDeclVisitor::getResultNode() const
{
	return this->resultNode;
}

ir_type* VariableDeclVisitor::getResultType() const
{
	return resultType;
}
