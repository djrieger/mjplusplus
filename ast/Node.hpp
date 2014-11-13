#ifndef NODE_HPP
#define NODE_HPP

#include <memory>
#include <string>

namespace ast
{
	class Node
	{
		public:
			virtual std::string toString() = 0;
	};
}

#endif
