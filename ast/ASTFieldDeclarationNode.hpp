#include "ASTClassMemberNode.hpp"

class ASTFieldDeclarationNode : public ASTClassMemberNode {
protected:
	ASTTypeIdentNode &type_and_name;

public:
	ASTFieldDeclarationNode(ASTTypeIdentNode &type_and_name);
}