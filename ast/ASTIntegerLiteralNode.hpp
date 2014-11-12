#include "ASTNode.hpp"

class ASTIntegerLiteralNode : public ASTNode {
private:
	std::string string_value;

public:
	ASTIntegerLiteralNode(std::string string_value);

	// Konstruktor überladen mit token?
}