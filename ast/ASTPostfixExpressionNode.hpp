#include "ASTNode.hpp"
#include <vector>

class ASTPostfixExpressionNode : public ASTExpressionNode {
private:
	ASTPrimaryExpressionNode &child;
	std::vector<ASTPostfixOpNode> postfix_op;

public:
	ASTPostfixExpressionNode(ASTPrimaryExpressionNode &child, std::vector<ASTPostfixOpNode> postfix_op);
}