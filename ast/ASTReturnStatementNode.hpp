#include "ASTNode.hpp"

class ASTReturnStatementNode : public ASTStatementNode {
private:
	ASTExpressionNode &expression;

public:
	ASTReturnStatementNode(ASTExpressionNode &expr);
}