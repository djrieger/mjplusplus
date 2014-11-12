#include "BinaryExpression.hpp"

ast::BinaryExpression::BinaryExpression(Expression& leftChild, Expression& rightChild, Token::Token_type operator_type)
	: leftChild(leftChild), rightChild(rightChild), operator_type(operator_type)
{

}

std::string ast::BinaryExpression::toString()
{
	//TODO operator_type -> string
	return leftChild.toString() + " + " + rightChild.toString();
}
