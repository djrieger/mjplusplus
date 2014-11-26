#ifndef AST_VISITOR_HPP
#define AST_VISITOR_HPP

#include "../globals.hpp"

namespace ast
{
	class Node;
}

class ASTVisitor
{
	public:
		virtual void visit(shptr<ast::Node const> node) = 0;
};

#endif