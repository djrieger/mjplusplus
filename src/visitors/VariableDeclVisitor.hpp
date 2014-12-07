#ifndef VARIABLE_DECL_VISITOR_HPP
#define VARIABLE_DECL_VISITOR_HPP

#include "VisitorWithResultNode.hpp"

class VariableDeclVisitor : public VisitorWithResultNode
{
	protected:
		ir_node* current_this;
		ir_node* store_value;

	public:
		VariableDeclVisitor(ir_node* current_this, ir_node* store_value = NULL);

		virtual void visit(shptr<ast::FieldDeclaration const> fieldDeclaration);
		virtual void visit(shptr<ast::LVDStatement const> lvdStatement);
		virtual void visit(shptr<ast::TypeIdent const> typeIdent);
};

#endif
