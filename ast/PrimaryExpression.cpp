#include "PrimaryExpression.hpp"

ast::PrimaryExpression::PrimaryExpression(std::unique_ptr<Node> &child, std::unique_ptr<std::vector<Expression>> &arguments) : child(std::move(child)), arguments(std::move(arguments))
{

}

std::string ast::PrimaryExpression::toString()
{
	std::string str = "";
	str += child->toString();
	str += "(";

	for (int i = 0, size = arguments->size(); i < size; ++i)
	{
		str += arguments->at(i).toString();
		str += (i < size - 1) ? ", " : "";
	}

	str += ")";
	return str;
}