#ifndef VARIABLE_DECL_VISITOR_HPP
#define VARIABLE_DECL_VISITOR_HPP

#include "VisitorWithResultNode.hpp"

class VariableDeclVisitor : public VisitorWithResultNode
{
	protected:
		//shptr<ast::IdentBase const> ident;//unneeded?
		ir_node* current_this;
		ir_type* resultType;
	public:
		VariableDeclVisitor(ir_node* current_this/*, shptr<ast::IdentBase const> ident*/);

		virtual void visit(shptr<ast::FieldDeclaration const> fieldDeclaration);
		virtual void visit(shptr<ast::LVDStatement const> lvdStatement);
		virtual void visit(shptr<ast::TypeIdent const> typeIdent);

		virtual ir_type* getResultType() const;
};

#endif
