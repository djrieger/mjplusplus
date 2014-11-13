#include "WhileStatement.hpp"

ast::WhileStatement::WhileStatement(std::unique_ptr<Expression> &condition, std::unique_ptr<Statement> &statement) :
	condition(std::move(condition)), statement(std::move(statement))
{

}

std::string ast::WhileStatement::toString()
{
	return "while (" + condition->toString() + ") {\n" + statement->toString() + "}\n";
}
