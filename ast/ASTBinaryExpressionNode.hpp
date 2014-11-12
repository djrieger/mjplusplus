#include "ASTNode.hpp"
#include <vector>

class ASTBinaryExpressionNode : public ASTExpressionNode {
private:
	ASTExpressionNode &leftChild;
	ASTExpressionNode &rightChild;
	Token::Token_type operator_type;

public:
	ASTBinaryExpressionNode(ASTExpressionNode &leftChild, ASTExpressionNode &rightChild, Token::Token_type operator_type);

}