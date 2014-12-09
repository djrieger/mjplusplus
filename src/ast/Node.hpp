#ifndef NODE_HPP
#define NODE_HPP

#include "../globals.hpp"
#include <iostream>
#include <memory>
#include <string>
#include "ASTVisitor.hpp"

namespace ast
{
	class Node : public std::enable_shared_from_this<Node>
	{
		public:
			/**
			 * writes a representation of the Node to out, including all children.
			 * special invokes "special" behavior for certain nodes
			 *     MethodDeclaration: print static keyword
			 *     Block & If: skip indentation
			 *     Expression: don't print surrounding parens
			 * in addition Block doesn't print newline after trailing } if ~indent is passed instead of indent
			 */
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const = 0;
			// TODO: Make pure virtual
			virtual void accept(ASTVisitor& visitor) const;
	};
}

#endif
