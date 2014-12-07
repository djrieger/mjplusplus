#include "VariableDeclVisitor.hpp"

VariableDeclVisitor::VariableDeclVisitor(ir_node* current_this/*, shptr<ast::IdentBase const> ident*/): /*ident(ident),*/ current_this(current_this)
{}

void VariableDeclVisitor::visit(shptr<ast::FieldDeclaration const> fieldDeclaration)
{

	//Our variable was declared as field => Our variable is a fieldaccess.
	std::cout << "vd visit FieldDecl" << std::endl;
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

	std::cout << "vd visit LVDDecl " << varName << std::endl;

	int pos = (*varMap)[varName];
	resultType = FirmInterface::getInstance().getType(lvdStatement->getDeclType());
	resultNode = get_value(pos, FirmInterface::getInstance().getMode(lvdStatement->getDeclType()));
}

void VariableDeclVisitor::visit(shptr<ast::TypeIdent const> typeIdent)
{
	//Our variable was declared as type ident => Our variable is a parameter (and we treat it as LV).
	auto varMap = FirmInterface::getInstance().getVarMap();
	auto varName = typeIdent->getName();

	std::cout << "vd visit TypeIdent/Parameter " << varName << std::endl;

	int pos = (*varMap)[varName];
	resultType = FirmInterface::getInstance().getType(typeIdent->getDeclType());
	resultNode = get_value(pos, FirmInterface::getInstance().getMode(typeIdent->getDeclType()));
}

ir_type* VariableDeclVisitor::getResultType() const
{
	return resultType;
}
