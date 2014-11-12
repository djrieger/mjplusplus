#include "BinaryExpression.hpp"

ast::BinaryExpression::BinaryExpression(Expression& leftChild, Expression& rightChild, Token::Token_type operator_type)
	:
{
	
}

std::string ast::BinaryExpression::toString() 
{
		return leftChild.toString() + operator_type.toString() + rightChild.toString();
}
