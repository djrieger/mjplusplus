#include "Node.hpp"
#include <vector>

class PrimaryExpression : public Expression
{
	private:
		Node& child;
		std::vector<Expression> arguments;

	public:
		PrimaryExpression(Node& child, std::vector<Expression> arguments);
}