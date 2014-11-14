#include "BinaryExpression.hpp"

ast::BinaryExpression::BinaryExpression(std::unique_ptr<Expression>& leftChild, std::unique_ptr<Expression>& rightChild, Token::Token_type const& operator_type)
	: leftChild(std::move(leftChild)), rightChild(std::move(rightChild)), operator_type(operator_type)
{

}

std::string ast::BinaryExpression::toString(unsigned int indent) const
{
	//TODO operator_type -> string
	return leftChild->toString(indent) + " + " + rightChild->toString(indent);
}
