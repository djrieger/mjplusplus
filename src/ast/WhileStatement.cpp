#include "../globals.hpp"
#include "WhileStatement.hpp"

ast::WhileStatement::WhileStatement(shptr<Expression> condition, shptr<Statement> statement) :
	condition(condition), statement(statement)
{

}

void ast::WhileStatement::toString(std::ostream& out, unsigned int indent, bool special) const
{
	if (!special)
		out << std::string(indent, '\t');

	bool block = statement && statement->getType() == Statement::Type::TYPE_BLOCK;

	out << "while (";
	condition->toString(out, indent, true);
	out << ')';
	out << (!statement || block ? ' ' : '\n');

	if (statement)
	{
		if (block)
			statement->toString(out, indent, true);
		else
			statement->toString(out, indent + 1);
	}
	else
		out << "{ }\n";
}

bool ast::WhileStatement::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	auto cond = condition->get_type(sa, symbolTable);

	if (!cond || *cond != ast::Type(ast::Type::Primitive_type::BOOLEAN))
		sa.printError("While condition is not boolean");

	if (statement)
		statement->analyze(sa, symbolTable);

	//TODO: we need to return true iff condition is "constant true"
	return false;
}
