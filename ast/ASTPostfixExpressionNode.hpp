#include "Node.hpp"
#include <vector>

namespace ast
{
	class PostfixExpression : public Expression
	{
		private:
			PrimaryExpression& child;
			std::vector<PostfixOp> postfix_op;

		public:
			PostfixExpression(PrimaryExpression& child, std::vector<PostfixOp> postfix_op);
	}
}