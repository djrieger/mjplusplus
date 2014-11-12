#include "ASTNode.hpp"

class ASTLVDStatementNode : public ASTStatementNode {
protected:
	ASTTypeIdentNode &type_ident;

public:
	ASTLVDStatementNode(ASTTypeIdentNode &type_ident);
}