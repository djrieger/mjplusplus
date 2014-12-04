#include "VariableDeclVisitor.hpp"

VariableDeclVisitor::VariableDeclVisitor(shptr<ast::pe::Ident const> ident): ident(ident)
{}

void VariableDeclVisitor::visit(shptr<ast::FieldDeclaration const> fieldDeclaration)
{
	//Our variable was declared as field => Our variable is a fieldaccess.
}

void VariableDeclVisitor::visit(shptr<ast::LVDStatement const> lvdStatement)
{
	//Our variable was declared as LV => Our variable is a LV.
	auto varMap = FirmInterface::getInstance().getVarMap();
	auto varName = lvdStatement->getIdent()->getName();
	int pos = (*varMap)[varName];
	resultNode = get_value(pos, mode_P);
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