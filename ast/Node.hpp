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
			virtual void toString(std::ostream& out, unsigned int indent) const = 0;
	};
}

#endif
