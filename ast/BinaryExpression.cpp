#include "BinaryExpression.hpp"

ast::BinaryExpression::BinaryExpression(std::unique_ptr<Expression>& leftChild, std::unique_ptr<Expression>& rightChild, Token::Token_type const& operator_type)
	: leftChild(std::move(leftChild)), rightChild(std::move(rightChild)), operator_type(operator_type)
{

}

std::string ast::BinaryExpression::toString()
{
	//TODO operator_type -> string
	return leftChild->toString() + " + " + rightChild->toString();
}
