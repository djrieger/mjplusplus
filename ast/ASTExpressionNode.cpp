#include "ASTNode.hpp"
#include "../lexer/token.hpp"

ASTExpressionNode::ASTExpressionNode(ASTExpressionNode& leftChild, ASTExpressionNode& rightChild, Token::Token_type operator_type) :
	leftChild(leftChild), rightChild(rightChild), operator_type(operator_type), is_unary(false) {

	}

ASTExpressionNode::ASTExpressionNode(ASTExpressionNode& child, Token::Token_type operator_type) :
	leftChild(child), rightChild(child), operator_type(operator_type), is_unary(true) {

	}
