#include "WhileStatement.hpp"

ast::WhileStatement::WhileStatement(Expression& condition, Statement& statement) :
	condition(condition), statement(statement)
{

}

std::string ast::WhileStatement::toString()
{
	return "while (" + condition.toString() + ") {" + std::endl + statement.toString + "}" + std::endl;
}