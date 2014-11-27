#ifndef EXPRESSION_VISITOR_HPP
#define EXPRESSION_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "../ast/BinaryExpression.hpp"

class ExpressionVisitor : public FirmVisitor
{
	public:
		//ExpressionVisitor(StatementVisitor &statementVisitor);
		virtual void visit(shptr<ast::be::Plus const> plusExpr);
};

#endif