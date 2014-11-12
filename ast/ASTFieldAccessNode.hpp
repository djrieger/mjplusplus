#include "ASTPostfixOpNode.hpp"

class ASTFieldAccessNode : public ASTPostfixOpNode {
private:
	ASTIdentNode &field_name;

public:
	ASTFieldAccessNode(ASTIdentNode &field_name);
}