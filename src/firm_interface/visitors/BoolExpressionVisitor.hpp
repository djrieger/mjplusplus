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

				void boolFromExpression(shptr<ast::Expression const> expression);
				void visitRelationalExpression(shptr<ast::be::BinaryExpression const> binaryExpression, ir_relation relation);
			public:
				BoolExpressionVisitor(ir_node* thenBlock, ir_node* elseBlock);

				// primary expressions
				virtual void visit(shptr<ast::pe::Bool const> boolExpr);
				virtual void visit(shptr<ast::pe::Ident const> identExpr);
				virtual void visit(shptr<ast::pe::MethodInvocation const> methodInvocationExpr);

				// unary expressions
				virtual void visit(shptr<ast::ue::Not const> notExpr);

				// binary expressions
				virtual void visit(shptr<ast::be::Eq const> eqEqExpr);
				virtual void visit(shptr<ast::be::AndAnd const> andAndExpr);
				virtual void visit(shptr<ast::be::EqEq const> eqEqExpr);
				virtual void visit(shptr<ast::be::GreaterThan const> greaterThanExpr);
				virtual void visit(shptr<ast::be::GreaterThanEq const> greaterThanEqExpr);
				virtual void visit(shptr<ast::be::Invalid const> invalidExpr);
				virtual void visit(shptr<ast::be::LessThan const> lessThanExpr);
				virtual void visit(shptr<ast::be::LessThanEq const> lessThanEqExpr);
				virtual void visit(shptr<ast::be::NotEq const> notEqExpr);
				virtual void visit(shptr<ast::be::OrOr const> orOrExpr);

				// postfix expression
				virtual void visit(shptr<ast::PostfixExpression const> postfixExpr);
		};
	}
}

#endif
