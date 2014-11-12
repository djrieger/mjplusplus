#include "ASTPostfixOpNode.hpp"

class ASTMethodInvocationNode : public ASTPostfixOpNode {
private:
	ASTIdentNode &method_name;
	std::vector<ASTExpressionNode> arguments;

public:
	ASTMethodInvocationNode(ASTIdentNode &method_name, std::vector<ASTExpressionNode> arguments);
}