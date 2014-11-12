#include "ASTNode.hpp"

class ASTBoolLiteralNode : public ASTNode {
private:
	bool value;

public:
	ASTBoolLiteralNode(bool value);

	// Konstruktor überladen mit token?
}