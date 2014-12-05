#ifndef VARIABLE_DECL_VISITOR_HPP
#define VARIABLE_DECL_VISITOR_HPP

#include "FirmVisitor.hpp"

class VariableDeclVisitor : public FirmVisitor
{
	protected:
		ir_node* current_this;
		ir_node* resultNode;
		ir_type* resultType;
	public:
		VariableDeclVisitor(ir_node* current_this);

		virtual void visit(shptr<ast::FieldDeclaration const> fieldDeclaration);
		virtual void visit(shptr<ast::LVDStatement const> lvdStatement);
		virtual void visit(shptr<ast::TypeIdent const> typeIdent);

		virtual ir_node* getResultNode() const;
		virtual ir_type* getResultType() const;
};

#endif
