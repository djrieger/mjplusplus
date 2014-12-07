#ifndef POSTFIX_OPS_VISITOR_HPP
#define POSTFIX_OPS_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "ExpressionVisitor.hpp"
#include "VisitorWithResultNode.hpp"

class PostfixOpsVisitor: public VisitorWithResultNode
{
	public:
		virtual void visit(shptr<ast::FieldAccess const> fieldAccess);
		virtual void visit(shptr<ast::ArrayAccess const> arrayAccess);
		virtual void visit(shptr<ast::MethodInvocation const> methodInvocation);

		void setStoreValue(ir_node* storeValue);
		PostfixOpsVisitor(ExpressionVisitor& expressionVisitor);
	protected:
		ir_node* storeValue;
		ExpressionVisitor& expressionVisitor;
};

#endif
