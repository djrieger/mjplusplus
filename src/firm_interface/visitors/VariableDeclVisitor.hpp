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

				/**
				 * the current this node
				 */
				ir_node* current_this;

				/**
				 * the store value
				 */
				ir_node* store_value;

				/**
				 * Evaluates the given VariableDeclaration by retrieving
				 * the variables position and type in the current context from the FirmInterface
				 * based on the variable's name/type.
				 * If doStore, then varNum is set to position.
				 * Else the returnNode is set to the value at the position.
				 * @param variableDeclaration the given VariableDeclaration
				 * @param varName             the variable's name
				 * @see FirmInterface
				 */
				void visitLVDStatementOrTypeIdent(shptr<ast::VariableDeclaration const> variableDeclaration, std::string varName);

			public:

				/**
				 * Constructs a VariableDeclVisitor with a current this node.
				 * @param the current this node
				 */
				VariableDeclVisitor(ir_node* current_this);

				/**
				 * Accesses the field that belongs to the visited FieldDeclaration.
				 * Sets the resultNode and -Type to the node and type of the field entity.
				 * @param fieldDeclaration the visited FieldDeclaration
				 */
				virtual void visit(shptr<ast::FieldDeclaration const> fieldDeclaration);

				/**
				 * Evaluates the visited LVDStatement by retrieving
				 * the variables position and type in the current context from the FirmInterface
				 * based on the variable's name/type.
				 * If doStore, then varNum is set to position.
				 * Else the returnNode is set to the value at the position.
				 * @param lvdStatement the visited LVDStatement
				 * @see FirmInterface
				 * @see VariableDeclVisitor::visitLVDStatementOrTypeIdent
				 */
				virtual void visit(shptr<ast::stmt::LVDStatement const> lvdStatement);

				/**
				 * Evaluates the visited TypeIdent by retrieving
				 * the variables position and type in the current context from the FirmInterface
				 * based on the variable's name/type.
				 * If doStore, then varNum is set to position.
				 * Else the returnNode is set to the value at the position.
				 * @param typeIdent the visited TypeIdent
				 * @see FirmInterface
				 * @see VariableDeclVisitor::visitLVDStatementOrTypeIdent
				 */
				virtual void visit(shptr<ast::TypeIdent const> typeIdent);
		};
	}
}

#endif
