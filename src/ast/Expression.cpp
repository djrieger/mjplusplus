#include "Expression.hpp"

ast::Expression::Expression(source_position_t position) : PositionAwareNode(position)
{
	;
}

void ast::Expression::accept(ASTVisitor& visitor) const
{
	visitor.visit(shared_from_this());
}
