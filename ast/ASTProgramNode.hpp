#include "ASTNode.hpp"

class ASTProgramNode : public ASTNode {
private:
	std::vector<ASTClassDeclarationNode> classes;

public:
	ASTProgramNode(std::vector<ASTClassDeclarationNode> classes);

}