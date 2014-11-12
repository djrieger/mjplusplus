#include "ASTPostfixOpNode.hpp"

class ASTArrayAccessNode : public ASTPostfixOpNode {
private:
	ASTExpressionNode& access_offset;

public:
	ASTArrayAccessNode(ASTExpressionNode& access_offset;);
}