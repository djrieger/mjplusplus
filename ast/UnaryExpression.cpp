#include "UnaryExpression.hpp"

ast::UnaryExpression::UnaryExpression(std::unique_ptr<PostfixExpression>& child, std::unique_ptr<std::vector<Unary_Operator>>& unary_operators) :
	child(std::move(child)), unary_operators(std::move(unary_operators))
{

}

void ast::UnaryExpression::toString(std::ostream& out, unsigned int indent) const
{
	bool last_was_minus = false;

	for (auto& unary_operator : *unary_operators)
	{
		switch (unary_operator)
		{
			case UnaryExpression::Unary_Operator::UNARY_NOT:
				out << "!";
				break;

			case UnaryExpression::Unary_Operator::UNARY_MINUS:
				out << (last_was_minus ? " -" : "-");
				break;
		}
	}

	child->toString(out, indent);
}
