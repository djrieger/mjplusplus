#ifndef CLASSMEMBER_H
#define CLASSMEMBER_H

#include "Node.hpp"

namespace ast
{
	class ClassMember : public Node
	{
		public:
			virtual std::string toString()
			{
				return "";
			};
	};
}

#endif
