#include "ASTNode.hpp"

class ASTInitLVDStatementNode : public ASTLVDStatementNode {
protected:
	ASTExpressionNode &init_expr;

public:
	ASTLVDStatementNode(ASTTypeIdentNode &type_ident, ASTExpressionNode &init_expr);
}