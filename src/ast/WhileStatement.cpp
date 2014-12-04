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

	if (!cond || *cond != ast::Type(ast::Type::Primitive_type::BOOLEAN)) //TODO: isBool();
		sa.reportError("While condition is not boolean", condition);

	bool definiteReturn = false;

	if (statement)
		definiteReturn = statement->analyze(sa, symbolTable);

	auto condPair = condition->constBool();
	return condPair.first && condPair.second && definiteReturn;
}

unsigned int ast::WhileStatement::countVariableDeclarations() const
{
	return statement ? statement->countVariableDeclarations() : 0;
}

int ast::WhileStatement::setVariablePositions(int pos) const
{

	if (statement)
		pos = statement->setVariablePositions(pos);

	return pos;
}

void ast::WhileStatement::accept(ASTVisitor& visitor) const
{
	visitor.visit(std::static_pointer_cast<WhileStatement const>(shared_from_this()));
}
