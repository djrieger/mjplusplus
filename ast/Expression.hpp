#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "Node.hpp"
#include "../lexer/token.hpp"

namespace ast
{
	class Expression : public Node
	{
		public:
			virtual std::string toString()
			{
				return "";
			};
	};
}

#endif
