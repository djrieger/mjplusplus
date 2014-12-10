#ifndef EXPRESSION_VISITOR_HPP
#define EXPRESSION_VISITOR_HPP

#include <functional>
#include "VisitorWithResultNode.hpp"
#include "../../ast/BinaryExpression.hpp"
#include "../../ast/PrimaryExpression.hpp"
#include "../../ast/UnaryExpression.hpp"
#include "../../ast/PostfixExpression.hpp"

namespace firm
{
	namespace visitor
	{
		class ExpressionVisitor : public VisitorWithResultNode
		{

			private:
				/*
				 * Create an ir_node for binExpr by calling createResultNode.
				 * createResultNode gets the nodes left and right as parameters, obtained by calling accept()
				 * on leftChild and rightChild of binExpr, respectively. It has to return a new ir_node
				 * which is then set as this->resultNode.
				 */
				void visitBinaryExpression(
				    shptr<ast::be::BinaryExpression const> binExpr,
				    std::function<ir_node* (ir_node*, ir_node*)> createResultNode
				);

				/**
				 * Create a then or else block for a boolean expression
				 * @param  block 		this block is matured and set as current block
				 * @param  boolValue	create a new const boolean Firm node with this value
				 * @param  exitBlock	prepend a new Jump node to this block
				 * @return	the const boolean Firm node with value boolValue
				 */
				ir_node* createThenOrElseBlock(ir_node* block, bool boolValue, ir_node* exitBlock);

				/*
				 * Evaluate boolean expression with help of BoolExpressionVisitor
				 */
				void visitBoolExpression(shptr<ast::Expression const> expression);

			public:
				ExpressionVisitor();

				// primary expressions
				virtual void visit(shptr<ast::pe::Bool const> boolExpr);
				virtual void visit(shptr<ast::pe::Ident const> identExpr);
				virtual void visit(shptr<ast::pe::Integer const> integerExpr);
				virtual void visit(shptr<ast::pe::MethodInvocation const> methodInvocationExpr);
				virtual void visit(shptr<ast::pe::NewArrayExpression const> newArrayExpr);
				virtual void visit(shptr<ast::pe::NewObjectExpression const> newObjectExpr);
				virtual void visit(shptr<ast::pe::Object const> objectExpr);

				// unary expressions
				virtual void visit(shptr<ast::ue::Neg const> negExpr);
				virtual void visit(shptr<ast::ue::Not const> notExpr);

				// binary expressions
				virtual void visit(shptr<ast::be::AndAnd const> andAndExpr);
				virtual void visit(shptr<ast::be::Eq const> eqExpr);
				virtual void visit(shptr<ast::be::EqEq const> eqEqExpr);
				virtual void visit(shptr<ast::be::GreaterThan const> greaterThanExpr);
				virtual void visit(shptr<ast::be::GreaterThanEq const> greaterThanEqExpr);
				virtual void visit(shptr<ast::be::Invalid const> invalidExpr);
				virtual void visit(shptr<ast::be::LessThan const> lessThanExpr);
				virtual void visit(shptr<ast::be::LessThanEq const> lessThanEqExpr);
				virtual void visit(shptr<ast::be::Minus const> minusExpr);
				virtual void visit(shptr<ast::be::Mod const> modExpr);
				virtual void visit(shptr<ast::be::Mult const> multExpr);
				virtual void visit(shptr<ast::be::NotEq const> notEqExpr);
				virtual void visit(shptr<ast::be::OrOr const> orOrExpr);
				virtual void visit(shptr<ast::be::Plus const> plusExpr);
				virtual void visit(shptr<ast::be::Slash const> slashExpr);

				// postfix expression
				virtual void visit(shptr<ast::po::PostfixExpression const> postfixExpr);
		};
	}
}

#endif