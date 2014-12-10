#ifndef BOOL_EXPRESSION_VISITOR_HPP
#define BOOL_EXPRESSION_VISITOR_HPP

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
		class BoolExpressionVisitor : public FirmVisitor
		{
			private:
				ir_node* thenBlock;
				ir_node* elseBlock;

				/**
				 * Visits the given Expression using an ExpressionVisitor and adds the necessary blocks
				 * using Cmp, Cond, Proj nodes and this visitor's then- and elseBlocks.
				 * This method is used in various visit-methods.
				 * @param expression the given Expression
				 * @see ExpressionVisitor
				 */
				void boolFromExpression(shptr<ast::Expression const> expression);

				/**
				 * Visits the children of the given BinaryExpression using
				 * an ExpressionVisitor and adds the necessary blocks
				 * using Cmp, Cond, Proj nodes and this visitor's then- and elseBlocks.
				 * This method is used in various relational visit-methods.
				 * @param binaryExpression the given BinaryExpression
				 * @param relation         the ir_relation
				 * @see ExpressionVisitor
				 */
				void visitRelationalExpression(shptr<ast::be::BinaryExpression const> binaryExpression, ir_relation relation);

			public:

				/**
				 * Constructs a BoolExpressionVisitor with the given then- and elseBlock.
				 * All boolean expressions are short circuit-evaluated.
				 * @param thenBlock
				 * @param elseBlock
				 */
				BoolExpressionVisitor(ir_node* thenBlock, ir_node* elseBlock);

				// primary expressions

				/**
				 * Visits a primary Bool expression and adds the necessary blocks to the graph.
				 * @param boolExpr the visited primary Bool expression
				 */
				virtual void visit(shptr<ast::pe::Bool const> boolExpr);

				/**
				 * Visits a primary Ident expression and adds the necessary blocks to the graph.
				 * @param identExpr the visited primary Ident expression
				 */
				virtual void visit(shptr<ast::pe::Ident const> identExpr);

				/**
				 * Visits a primary MethodInvocation expression and adds the necessary blocks to the graph.
				 * @param methodInvocationExpr the visited primary Ident expression
				 */
				virtual void visit(shptr<ast::pe::MethodInvocation const> methodInvocationExpr);

				// unary expressions

				/**
				 * Visits a unary Not expression, swaps the necessary blocks to the graph and
				 * then evaluates the child.
				 * @param notExpr the visited unary Not expression
				 */
				virtual void visit(shptr<ast::ue::Not const> notExpr);

				// binary expressions

				/**
				 * Visits the binary '=' (Eq-)expression and adds the necessary blocks to the graph.
				 * @param eqExpr the visited Eq expression
				 */
				virtual void visit(shptr<ast::be::Eq const> eqExpr);

				/**
				 * Visits the binary '&&' (AndAnd-)expression and adds the necessary blocks to the graph.
				 * @param andAndExpr the visited AndAnd expression
				 */
				virtual void visit(shptr<ast::be::AndAnd const> andAndExpr);

				/**
				 * Visits the binary '||' (OrOr-)expression and adds the necessary blocks to the graph.
				 * @param orOrExpr the visited OrOr expression
				 */
				virtual void visit(shptr<ast::be::OrOr const> orOrExpr);

				/**
				 * Visits the binary '==' (EqEq-)expression and and adds the necessary blocks to the graph.
				 * Uses ir_relation::ir_relation_equal
				 * @param eqEqExpr the visited EqEq expression
				 */
				virtual void visit(shptr<ast::be::EqEq const> eqEqExpr);

				/**
				 * Visits the binary '>' (GreaterThan-)expression and adds the necessary blocks to the graph.
				 * Uses ir_relation::ir_relation_greater.
				 * @param greaterThanExpr the visited GreaterThan expression
				 */
				virtual void visit(shptr<ast::be::GreaterThan const> greaterThanExpr);

				/**
				 * Visits the binary '>=' (GreaterThanEq-)expression and adds the necessary blocks to the graph.
				 * Uses ir_relation::ir_relation_greater_equal.
				 * @param greaterThanEqExpr the visited GreaterThanEq expression
				 */
				virtual void visit(shptr<ast::be::GreaterThanEq const> greaterThanEqExpr);

				/**
				 * Prints an error. There shouldn't be an Invalid expression in the AST.
				 * @param invalidExpr the visited Invalid expression
				 */
				virtual void visit(shptr<ast::be::Invalid const> invalidExpr);

				/**
				 * Visits the binary '<' (LessThan-)expression and adds the necessary blocks to the graph.
				 * Uses ir_relation::ir_relation_less.
				 * @param lessThanExpr the visited LessThan expression
				 */
				virtual void visit(shptr<ast::be::LessThan const> lessThanExpr);

				/**
				 * Visits the binary '<=' (LessThanEq-)expression and adds the necessary blocks to the graph.
				 * Uses ir_relation::ir_relation_less_equal.
				 * @param lessThanEqExpr the visited LessThanEq expression
				 */
				virtual void visit(shptr<ast::be::LessThanEq const> lessThanEqExpr);

				/**
				 * Visits the binary '!=' (NotEq-)expression and adds the necessary blocks to the graph.
				 * Uses ir_relation::ir_relation_unordered_less_greater.
				 * @param notEqExpr the visited NotEq expression
				 */
				virtual void visit(shptr<ast::be::NotEq const> notEqExpr);


				// postfix expression

				/**
				 * Visits a PostfixExpression and adds the necessary blocks to the graph.
				 * @param postfixExpr the visited PostfixExpression
				 */
				virtual void visit(shptr<ast::po::PostfixExpression const> postfixExpr);
		};
	}
}

#endif
