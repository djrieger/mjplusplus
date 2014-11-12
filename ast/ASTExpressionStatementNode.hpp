#include "ASTNode.hpp"

class ASTExpressionStatementNode : public ASTStatementNode {
private:
	ASTExpressionNode &expression;

public:
	ASTExpressionStatementNode(ASTExpressionNode &expr);
}