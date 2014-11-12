#include "ASTNode.hpp"

class ASTObjectLiteralNode : public ASTNode {
private:
	Object_Type object_type;

public:
	ASTObjectLiteralNode(Object_Type object_type);

	enum Object_Type {
		NULL_OBJECT,
		THIS_OBJECT
	};
}