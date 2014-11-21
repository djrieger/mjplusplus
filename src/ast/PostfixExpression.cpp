#include "../globals.hpp"
#include "PostfixExpression.hpp"

ast::PostfixExpression::PostfixExpression(shptr<Expression> child, shptr<vec<shptr<PostfixOp>>> postfix_op) : child(child), postfix_op(postfix_op)
{

}

void ast::PostfixExpression::toString(std::ostream& out, unsigned int indent, bool special) const
{
	if (!special && !postfix_op->empty())
		out << '(';

	if (postfix_op->size() > 1)
		out << std::string(postfix_op->size() - 1, '(');

	child->toString(out, indent);

	for (auto it = postfix_op->begin(); it != postfix_op->end();)
	{
		(**it).toString(out, indent);

		if (++it != postfix_op->end())
			out << ')';
	}

	if (!special && !postfix_op->empty())
		out << ')';
}

shptr<ast::Type> ast::PostfixExpression::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const {
	//TODO: implement this like in Unary- and PrimaryExpression
	return shptr<ast::Type>();
}