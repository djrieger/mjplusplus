#include "Node.hpp"

void ast::Node::accept(ASTVisitor& visitor) const
{
	visitor.visit(shared_from_this());
}
