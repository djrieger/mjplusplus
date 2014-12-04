#include "VariableDeclVisitor.hpp"

VariableDeclVisitor::VariableDeclVisitor(shptr<ast::pe::Ident const> ident): ident(ident)
{}

void VariableDeclVisitor::visit(shptr<ast::FieldDeclaration const> fieldDeclaration) {
	// public int field;
	//FirmInterface::getInstance().getFieldEntity()
	// Class node { public Node parent; 
	// parent.foo();
}

void VariableDeclVisitor::visit(shptr<ast::LVDStatement const> lvdStatement) {
	// int i
	auto varMap = FirmInterface::getInstance().getVarMap();
	auto varName = lvdStatement->getIdent()->getName();
	int pos = (*varMap)[varName];
	resultNode = get_value(pos, mode_P);
}

void VariableDeclVisitor::visit(shptr<ast::TypeIdent const> typeIdent) {
  // $System System
}

ir_node* VariableDeclVisitor::getResultNode() const 
{
	return this->resultNode;
}