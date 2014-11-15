#include "PostfixExpression.hpp"

ast::PostfixExpression::PostfixExpression(std::unique_ptr<pe::PrimaryExpression>& child, std::unique_ptr<std::vector<std::unique_ptr<PostfixOp>>>& postfix_op) : child(std::move(child)), postfix_op(std::move(postfix_op))
{

}

void ast::PostfixExpression::toString(std::ostream& out, unsigned int indent) const
{
	child->toString(out, indent);

	for (auto& p : *postfix_op)
		p->toString(out, indent);
}
