#include "ASTNode.hpp"

class ASTIfElseStatementNode : public ASTIfStatementNode {
protected:
	ASTStatementNode &elseStatement;

public:
	ASTIfElseStatementNode(ASTExpressionNode &condition, ASTStatementNode &thenStatement, ASTStatementNode &elseStatement);
}