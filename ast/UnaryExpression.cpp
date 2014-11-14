#include "UnaryExpression.hpp"

ast::UnaryExpression::UnaryExpression(std::unique_ptr<PostfixExpression>& child, std::unique_ptr<std::vector<Unary_Operator>>& unary_operators) :
	child(std::move(child)), unary_operators(std::move(unary_operators))
{

}

std::string ast::UnaryExpression::toString(unsigned int indent) const
{
	std::string s = "";

	for (auto& unary_operator : *unary_operators)
	{
		switch (unary_operator)
		{
			case UnaryExpression::Unary_Operator::UNARY_NOT:
				s += "!";
				break;

			case UnaryExpression::Unary_Operator::UNARY_MINUS:
				s += "-";
				break;
		}
	}

	s += child->toString(indent);
	return s;
}
