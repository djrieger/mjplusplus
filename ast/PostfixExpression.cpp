#include "PostfixExpression.hpp"

ast::PostfixExpression::PostfixExpression(std::unique_ptr<pe::PrimaryExpression>& child, std::unique_ptr<std::vector<std::unique_ptr<PostfixOp>>>& postfix_op) : child(std::move(child)), postfix_op(std::move(postfix_op))
{

}

void ast::PostfixExpression::toString(std::ostream& out, unsigned int indent, bool special) const
{
	if (special)
		out << '(';

	if (postfix_op->size() > 1)
		out << std::string(postfix_op->size() - 1, '(');

	child->toString(out, indent);

	for (auto it = postfix_op->begin(); it != postfix_op->end();)
	{
		(**it).toString(out, indent);

		if (++it != postfix_op->end())
			out << ')';
	}

	if (special)
		out << ')';
}
