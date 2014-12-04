#ifndef VARIABLE_DECL_VISITOR_HPP
#define VARIABLE_DECL_VISITOR_HPP

#include "FirmVisitor.hpp"

class VariableDeclVisitor : public FirmVisitor
{
	protected:
		shptr<ast::pe::Ident const> ident;
		ir_node* resultNode;
	public:
		VariableDeclVisitor(shptr<ast::pe::Ident const> ident);

		virtual void visit(shptr<ast::FieldDeclaration const> fieldDeclaration);
		virtual void visit(shptr<ast::LVDStatement const> lvdStatement);
		virtual void visit(shptr<ast::TypeIdent const> typeIdent);

		virtual ir_node* getResultNode() const;
};

#endif