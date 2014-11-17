#ifndef TYPE_HPP
#define TYPE_HPP

#include "../globals.hpp"
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