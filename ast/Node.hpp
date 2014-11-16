#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>
#include <memory>
#include <string>

namespace ast
{
	class Node
	{
		public:
			/**
			 * writes a representation of the Node to out, including all children.
			 * @param special invokes "special" behavior for certain nodes
			 *     Block: don't print newline after trailing }
			 *     Expression: don't print surrounding parens
			 *     MethodDeclaration: print static keyword
			 *     If: skip indetation
			 */
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const = 0;
	};
}

#endif
