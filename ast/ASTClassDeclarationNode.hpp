#include "ASTNode.hpp"

class ASTClassDeclarationNode : public ASTNode {
private:
	ASTIdentNode &class_name;
	std::vector<ASTClassMemberNode> members;

public:
	ASTClassDeclarationNode(ASTIdentNode &class_name, std:vector<ASTClassMemberNode> members);
}