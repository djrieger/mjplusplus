#include "PrimaryExpression.hpp"

ast::PrimaryExpression::PrimaryExpression(Node& child, std::vector<Expression> arguments) : child(child), arguments(arguments)
{

}

std::string ast::PrimaryExpression::toString()
{
	std::string str = "";
	str += child.toString();
	str += "(";

	for (int i = 0, size = arguments.size(); i < size; ++i)
	{
		str += arguments[i].toString();
		str += (i < size - 1) ? ", " : "";
	}

	str += ")";
	return str;
}