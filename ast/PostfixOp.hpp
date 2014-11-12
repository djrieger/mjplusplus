#ifndef POSTFIXOP_HPP
#define POSTFIXOP_HPP
#include "Node.hpp"

namespace ast
{
	class PostfixOp : public Node
	{
		public:
			virtual std::string toString()
			{
				return "";
			}
	};
}
#endif // POSTFIXOP_HPP