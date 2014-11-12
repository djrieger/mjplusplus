#include "ASTNode.hpp"

class ASTIfStatementNode : public ASTStatementNode {
protected:
	ASTExpressionNode &condition;
	ASTStatementNode &thenStatement;

public:
	ASTIfStatementNode(ASTExpressionNode &condition, ASTStatementNode &thenStatement);
}