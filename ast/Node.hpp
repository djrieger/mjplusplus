#ifndef NODE_HPP
#define NODE_HPP

#include <memory>
#include <string>

namespace ast
{
	class Node
	{
		public:
			virtual std::string toString(unsigned int indent) const = 0;
	};
}

#endif
