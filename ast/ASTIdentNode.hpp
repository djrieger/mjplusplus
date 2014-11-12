#include "ASTNode.hpp"

class ASTIdentNode : public ASTNode {
private:
	std::string string_value;

public:
	ASTIdentNode(std::string string_value);

	// Konstruktor überladen mit token?
}