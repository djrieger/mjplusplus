#include "../globals.hpp"
#include "ReturnStatement.hpp"

ast::ReturnStatement::ReturnStatement(shptr<Expression> expression):
	PositionAwareNode(expression->getPosition()), expression(expression)
{

}

ast::ReturnStatement::ReturnStatement(source_position_t position): PositionAwareNode(position)
{

}

void ast::ReturnStatement::toString(std::ostream& out, unsigned int indent, bool) const
{
	out << std::string(indent, '\t') << "return";

	if (expression)
	{
		out << ' ';
		expression->toString(out, indent, true);
	}

	out << ";\n";
}

bool ast::ReturnStatement::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	auto ret_type = Symbol::makeSymbol("return", symbolTable->getCurrentScope())->getCurrentDefinition()->getType();
	auto expr_type = std::make_shared<ast::Type>(ast::Type::Primitive_type::VOID);

	if (expression)
	{
		if (*ret_type == *expr_type)
			sa.reportError("Method returns void, but return statement has expression.", this->expression);
		else
		{
			expr_type = expression->get_type(sa, symbolTable);

			if (expr_type)
			{
				if (!(*ret_type == *expr_type ||
				        (ret_type->isRefType() && expr_type->getPrimitiveType() == ast::Type::Primitive_type::NULL_TYPE)))
					sa.reportError("Mismatched types in return: $type{" + ret_type->getName() + "} and {" + expr_type->getName() + "}", this->expression);
			}
		}
	}
	else if (*ret_type != *expr_type)
		sa.reportError("Method returns $type{" + ret_type->getName() + "} but return statement has no expression", this->getPosition());

	return true;
}

void ast::ReturnStatement::accept(ASTVisitor& visitor) const
{
	visitor.visit(shared_from_this());
}