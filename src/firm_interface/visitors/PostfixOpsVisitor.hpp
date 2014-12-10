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
				ExpressionVisitor& expressionVisitor;

			public:
				PostfixOpsVisitor(ExpressionVisitor& expressionVisitor);

				virtual void visit(shptr<ast::po::FieldAccess const> fieldAccess);
				virtual void visit(shptr<ast::po::ArrayAccess const> arrayAccess);
				virtual void visit(shptr<ast::po::MethodInvocation const> methodInvocation);
		};
	}
}

#endif
