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

				/**
				 * Create an ir_node for the visited BinaryExpression by calling the visited function.
				 * The function gets the nodes left and right as parameters, obtained by visiting the
				 * leftChild and rightChild of the BinaryExpression, respectively.
				 * It has to return a new ir_node which is then set as this->resultNode.
				 * @param binExpr the visited BinaryExpression
				 * @param createResultNode the visited function
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

				/**
				 * Evaluate boolean expression with help of a BoolExpressionVisitor.
				 * @param expression the boolean expression
				 * @see BoolExpressionVisitor
				 */
				void visitBoolExpression(shptr<ast::Expression const> expression);

			public:
				ExpressionVisitor();

				// primary expressions

				/**
				 * Reads the bool-value of the visited Bool-expression,
				 * creates a boolean constant node and sets that to this visitor's resultNode.
				 * @param boolExpr the visited Bool-expression
				 */
				virtual void visit(shptr<ast::pe::Bool const> boolExpr);

				/**
				 * Checks the declaration of the visited Ident-expression.
				 * If it has a declaration, it is evaluated with a VariableDeclVisitor.
				 * The resultNode and -Type are set the evaluated values.
				 * If it has no declaration we got a System (from System.out.println(x)) and
				 * the appropriate nodes are created.
				 * @param identExpr the visited Ident-expression
				 */
				virtual void visit(shptr<ast::pe::Ident const> identExpr);

				/**
				 * Reads the integer-value of the visited Integer-expression,
				 * creates an integer constant node and sets that to this visitor's resultNode.
				 * @param integerExpr the visited Integer-expression
				 */
				virtual void visit(shptr<ast::pe::Integer const> integerExpr);

				/**
				 * Creates a node for the method call with the help of FirmInterface.
				 * Sets the resultNode and -Type to node and type of the call.
				 * @param methodInvocationExpr the visited primary MethodInvocation
				 * @see FirmInterface
				 */
				virtual void visit(shptr<ast::pe::MethodInvocation const> methodInvocationExpr);

				/**
				 * Evaluates the size expression of the visited NewArrayExpression. Then creates
				 * an appropriate node with the correct size and uses that to set this visitor's
				 * resultNode and -Type. Uses FirmInterfaces calloc-call method.
				 * @param newArrayExpr the visited NewArrayExpression
				 * @see FirmInterface
				 */
				virtual void visit(shptr<ast::pe::NewArrayExpression const> newArrayExpr);

				/**
				 * Creates a node with the appropriate type (based on the visited
				 * NewObjectExpression's type) and uses that to set this visitor's
				 * resultNode and -Type. Uses FirmInterfaces calloc-call method.
				 * @param newObjectExpr the visited NewObjectExpression
				 * @see FirmInterface
				 */
				virtual void visit(shptr<ast::pe::NewObjectExpression const> newObjectExpr);

				/**
				 * If the visited Object expression is a 'this', then this visitor's
				 * resultNode is set to the node currently stored at pos 0.
				 * If it is a 'null', then the resultNode is set to a created NullPointerNode.
				 * @param objectExpr the visited Object expression
				 */
				virtual void visit(shptr<ast::pe::Object const> objectExpr);

				// unary expressions

				/**
				 * Evaluates the child of the visited Neg expression and then creates a Minus node
				 * with the node of the child, if the size of the Neg is uneven.
				 * @param negExpr the visited Neg expression
				 */
				virtual void visit(shptr<ast::ue::Neg const> negExpr);

				/**
				 * Evaluates the visited Not expression with the help of a BoolExpressionVisitor.
				 * @param notExpr the visited Not expression
				 * @see BoolExpressionVisitor
				 */
				virtual void visit(shptr<ast::ue::Not const> notExpr);

				// binary expressions

				/**
				 * [visit description]
				 * @param eqExpr [description]
				 */
				virtual void visit(shptr<ast::be::Eq const> eqExpr);

				/**
				 * Evaluates the visited AndAnd expression with the help of a BoolExpressionVisitor.
				 * @param andAndExpr the visited AndAnd expression
				 * @see BoolExpressionVisitor
				 */
				virtual void visit(shptr<ast::be::AndAnd const> andAndExpr);

				/**
				 * Evaluates the visited OrOr expression with the help of a BoolExpressionVisitor.
				 * @param orOrExpr the visited OrOr expression
				 * @see BoolExpressionVisitor
				 */
				virtual void visit(shptr<ast::be::OrOr const> orOrExpr);

				/**
				 * Evaluates the visited EqEq expression with the help of a BoolExpressionVisitor.
				 * @param eqEqExpr the visited EqEq expression
				 * @see BoolExpressionVisitor
				 */
				virtual void visit(shptr<ast::be::EqEq const> eqEqExpr);

				/**
				 * Evaluates the visited NotEq expression with the help of a BoolExpressionVisitor.
				 * @param notEqExpr the visited NotEq expression
				 * @see BoolExpressionVisitor
				 */
				virtual void visit(shptr<ast::be::NotEq const> notEqExpr);

				/**
				 * Evaluates the visited GreaterThan expression with the help of a BoolExpressionVisitor.
				 * @param greaterThanExpr the visited GreaterThan expression
				 * @see BoolExpressionVisitor
				 */
				virtual void visit(shptr<ast::be::GreaterThan const> greaterThanExpr);

				/**
				 * Evaluates the visited GreaterThanEq expression with the help of a BoolExpressionVisitor.
				 * @param greaterThanEqExpr the visited GreaterThanEq expression
				 * @see BoolExpressionVisitor
				 */
				virtual void visit(shptr<ast::be::GreaterThanEq const> greaterThanEqExpr);

				/**
				 * Evaluates the visited LessThan expression with the help of a BoolExpressionVisitor.
				 * @param lessThanExpr the visited LessThan expression
				 * @see BoolExpressionVisitor
				 */
				virtual void visit(shptr<ast::be::LessThan const> lessThanExpr);

				/**
				 * Evaluates the visited AndAnd expression with the help of a BoolExpressionVisitor.
				 * @param andAndExpr the visited AndAnd expression
				 * @see BoolExpressionVisitor
				 */
				virtual void visit(shptr<ast::be::LessThanEq const> lessThanEqExpr);

				/**
				 * Evaluates the children of the visited Plus expression and creates a new Add-node
				 * with the evaluated nodes and sets this visitor's resultNode to that.
				 * @param plusExpr the visited Plus expression
				 */
				virtual void visit(shptr<ast::be::Plus const> plusExpr);

				/**
				 * Evaluates the children of the visited Minus expression and creates a new Sub-node
				 * with the evaluated nodes and sets this visitor's resultNode to that.
				 * @param minusExpr the visited Minus expression
				 */
				virtual void visit(shptr<ast::be::Minus const> minusExpr);

				/**
				 * Evaluates the children of the visited Mod expression and creates a new Mod-node
				 * with the evaluated nodes and sets this visitor's resultNode to that.
				 * @param modExpr the visited Mod expression
				 */
				virtual void visit(shptr<ast::be::Mod const> modExpr);

				/**
				 * Evaluates the children of the visited Mult expression and creates a new Mul-node
				 * with the evaluated nodes and sets this visitor's resultNode to that.
				 * @param multExpr the visited Mult expression
				 */
				virtual void visit(shptr<ast::be::Mult const> multExpr);

				/**
				 * Evaluates the children of the visited Slash expression and creates a new DivRL-node
				 * with the evaluated nodes and sets this visitor's resultNode to that.
				 * @param slashExpr the visited Slash expression
				 */
				virtual void visit(shptr<ast::be::Slash const> slashExpr);

				/**
				 * Prints out an error message, since it shouldn't happen that this visits
				 * an Invalid expression.
				 * @param invalidExpr
				 */
				virtual void visit(shptr<ast::be::Invalid const> invalidExpr);


				// postfix expression

				/**
				 * Evaluates the child of the visited PostfixExpression and then uses a
				 * PostfixOpsVisitor to also evalute its PostfixOps.
				 * @param postfixExpr the visited PostfixExpression
				 * @see PostfixOpsVisitor
				 */
				virtual void visit(shptr<ast::po::PostfixExpression const> postfixExpr);
		};
	}
}

#endif
