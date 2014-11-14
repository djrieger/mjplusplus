#ifndef TYPE_HPP
#define TYPE_HPP

#include "Node.hpp"

namespace ast
{
	class Type : public Node
	{

		public:
			enum Primitive_type
			{
				NONE,
				INT,
				BOOLEAN,
				VOID
			};

	};
}

#endif