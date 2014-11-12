#include "ASTClassMemberNode.hpp"

class ASTMethodDeclarationNode : public ASTClassMemberNode {
protected:
	ASTTypeIdentNode &return_type_and_name;
	std::vector<ASTTypeIdentNode> parameters;
	ASTStatementNode &block;

public:
	ASTMethodDeclarationNode(ASTTypeIdentNode &return_type_and_name, std::vector<ASTTypeIdentNode> parameters, ASTStatementNode &block);
}