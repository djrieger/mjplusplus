#ifndef POSTFIX_OPS_VISITOR_HPP
#define POSTFIX_OPS_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "ExpressionVisitor.hpp"

class PostfixOpsVisitor: public FirmVisitor
{
	public:
		virtual void visit(shptr<ast::FieldAccess const> fieldAccess);
		virtual void visit(shptr<ast::ArrayAccess const> arrayAccess);
		virtual void visit(shptr<ast::MethodInvocation const> methodInvocation);

		virtual ir_node* getResultNode() const;
		virtual ir_type* getResultType() const;
		PostfixOpsVisitor(ExpressionVisitor& expressionVisitor);
	protected:
		ir_node* resultNode;
		ir_type* resultType;
		ExpressionVisitor& expressionVisitor;
};

#endif
