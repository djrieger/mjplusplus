#include "ASTNode.hpp"

class ASTWhileStatementNode : public ASTStatementNode {
protected:
	ASTExpressionNode &condition;
	ASTStatementNode &statement;

public:
	ASTWhileStatementNode(ASTExpressionNode &condition, ASTStatementNode &statement);
}