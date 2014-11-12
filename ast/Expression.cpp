#include "Node.hpp"
#include "../lexer/token.hpp"

Expression::Expression(Expression& leftChild, Expression& rightChild, Token::Token_type operator_type) :
	leftChild(leftChild), rightChild(rightChild), operator_type(operator_type), is_unary(false)
{

}
}

Expression::Expression(Expression& child, Token::Token_type operator_type) :
	leftChild(child), rightChild(child), operator_type(operator_type), is_unary(true)
{

}
}
