#ifndef POSTFIX_OPS_VISITOR_HPP
#define POSTFIX_OPS_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "ExpressionVisitor.hpp"
#include "VisitorWithResultNode.hpp"

namespace firm
{
	namespace visitor
	{
		class PostfixOpsVisitor: public VisitorWithResultNode
		{
			protected:
				/**
				 * The calling ExpressionVisitor
				 */
				ExpressionVisitor& expressionVisitor;

			public:

				/**
				 * Constructs a PostfixOpsVisitor from the visited ExpressionVisitor.
				 * @param expressionVisitor the visited ExpressionVisitor
				 */
				PostfixOpsVisitor(ExpressionVisitor& expressionVisitor);

				/**
				 * Evaluates the visited FieldAccess by evaluating its declaration with the help
				 * of a VariableDeclVisitor, that is initialized with the current resultNode of
				 * this expressionVisitor.
				 * @param fieldAccess the visited FieldAccess
				 * @see VariableDeclVisitor
				 */
				virtual void visit(shptr<ast::po::FieldAccess const> fieldAccess);

				/**
				 * Gets the node of the array from the expressionVisitor,
				 * then evaluates the index-expression using a new ExpressionVisitor and
				 * then creates the address node of the element.
				 * @param arrayAccess the visited ArrayAccess
				 */
				virtual void visit(shptr<ast::po::ArrayAccess const> arrayAccess);

				/**
				 * Creates a node for the method call with the help of FirmInterface.
				 * Sets the resultNode and -Type to node and type of the call.
				 * @param methodInvocation the visited MethodInvocation
				 * @see FirmInterface
				 */
				virtual void visit(shptr<ast::po::MethodInvocation const> methodInvocation);
		};
	}
}

#endif
