#include "Node.hpp"
#include <vector>

namespace ast
{
	class PrimaryExpression : public Expression
	{
		private:
			Node& child;
			std::vector<Expression> arguments;

		public:
			PrimaryExpression(Node& child, std::vector<Expression> arguments);
	}
}