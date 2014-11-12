#include "Node.hpp"
#include <vector>

class PostfixExpression : public Expression
{
	private:
		PrimaryExpression& child;
		std::vector<PostfixOp> postfix_op;

	public:
		PostfixExpression(PrimaryExpression& child, std::vector<PostfixOp> postfix_op);
}