#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "Node.hpp"

namespace ast
{
	class Statement : public Node
	{
		public:
			enum Type
			{
				TYPE_SINGLE,
				TYPE_BLOCK,
				TYPE_IF
			};

			virtual Type getType() const;

	};
}

#endif
