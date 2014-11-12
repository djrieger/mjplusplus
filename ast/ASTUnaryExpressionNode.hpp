#include "ASTNode.hpp"
#include <vector>

class ASTUnaryExpressionNode : public ASTExpressionNode {
private:
	ASTPostfixExpressionNode &child;
	std::vector<Unary_Operator> unary_operators;
	

public:
	ASTUnaryExpressionNode(ASTPostfixExpressionNode &child, std::vector<Unary_Operator> unary_operators);

	enum Unary_Operator {
		UNARY_NOT,
		UNARY_MINUS
	};
}