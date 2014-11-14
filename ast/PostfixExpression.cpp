#include "PostfixExpression.hpp"

ast::PostfixExpression::PostfixExpression(std::unique_ptr<pe::PrimaryExpression>& child, std::unique_ptr<std::vector<std::unique_ptr<PostfixOp>>>& postfix_op) : child(std::move(child)), postfix_op(std::move(postfix_op))
{

}

std::string ast::PostfixExpression::toString(unsigned int indent) const
{
	std::string str = child->toString(indent);

	for (auto& p : *postfix_op)
		str += p->toString(indent);

	return str;
}
