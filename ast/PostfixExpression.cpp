#include "PostfixExpression.hpp"

ast::PostfixExpression::PostfixExpression(PrimaryExpression& child, std::vector<PostfixOp> postfix_op) : child(child), postfix_op(postfix_op)
{

}

std::string ast::PostfixExpression::toString()
{
	std::string str = child.toString();

	for (p : postfix_op)
		str += p.toString();

	return str;
}