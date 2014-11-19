#ifndef CLASSMEMBER_HPP
#define CLASSMEMBER_HPP

#include "Node.hpp"

namespace ast
{
	class ClassMember : public Node
	{

		public:
			virtual std::string getName() const = 0;
	};
}

#endif
