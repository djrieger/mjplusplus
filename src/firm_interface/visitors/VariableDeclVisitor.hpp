#ifndef VARIABLE_DECL_VISITOR_HPP
#define VARIABLE_DECL_VISITOR_HPP

#include "VisitorWithResultNode.hpp"

namespace firm
{
	namespace visitor
	{
		class VariableDeclVisitor : public VisitorWithResultNode
		{
			protected:
				ir_node* current_this;
				ir_node* store_value;

				void visitLVDStatementOrTypeIdent(shptr<ast::VariableDeclaration const> variableDeclaration, std::string varName);

			public:
				VariableDeclVisitor(ir_node* current_this, ir_node* store_value = NULL);

				virtual void visit(shptr<ast::FieldDeclaration const> fieldDeclaration);
				virtual void visit(shptr<ast::stmt::LVDStatement const> lvdStatement);
				virtual void visit(shptr<ast::TypeIdent const> typeIdent);
		};
	}
}

#endif
