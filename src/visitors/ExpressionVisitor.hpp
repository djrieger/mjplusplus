#ifndef EXPRESSION_VISITOR_HPP
#define EXPRESSION_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "../ast/BinaryExpression.hpp"

class ExpressionVisitor : public FirmVisitor
{

	private:
		ir_node* resultNode;

	public:

		//ExpressionVisitor(StatementVisitor &statementVisitor);

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
		virtual void visit(shptr<ast::PostfixExpression> postfixExpr);
};

#endif
