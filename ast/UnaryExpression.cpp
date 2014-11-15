#include "UnaryExpression.hpp"

ast::UnaryExpression::UnaryExpression(std::unique_ptr<PostfixExpression>& child, std::unique_ptr<std::vector<Unary_Operator>>& unary_operators) :
	child(std::move(child)), unary_operators(std::move(unary_operators))
{

}

void ast::UnaryExpression::toString(std::ostream& out, unsigned int indent) const
{
	for (auto& unary_operator : *unary_operators)
	{
		switch (unary_operator)
		{
			case UnaryExpression::Unary_Operator::UNARY_NOT:
				out << "!";
				break;

			case UnaryExpression::Unary_Operator::UNARY_MINUS:
				out << "-";
				break;
		}
	}

	child->toString(out, indent);
}
