#include "ASTNode.hpp"
#include <vector>

class ASTPrimaryExpressionNode : public ASTExpressionNode {
private:
	ASTNode &child;
	std::vector<ASTExpressionNode> arguments;

public:
	ASTPrimaryExpressionNode(ASTNode &child, std::vector<ASTExpressionNode> arguments);
}