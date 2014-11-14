#ifndef NODE_HPP
#define NODE_HPP

#include <memory>
#include <string>

namespace ast
{
	class Node
	{
		public:
			virtual std::string toString() const = 0;
	};
}

#endif
