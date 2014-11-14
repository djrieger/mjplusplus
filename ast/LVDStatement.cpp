#include "LVDStatement.hpp"

ast::LVDStatement::LVDStatement(std::unique_ptr<TypeIdent>& type_ident)
	: type_ident(std::move(type_ident))
{

}
ast::LVDStatement::LVDStatement(std::unique_ptr<TypeIdent>& type_ident, std::unique_ptr<Expression>& init_expr)
	: type_ident(std::move(type_ident)), init_expr(std::move(init_expr))
{
	;
}

std::string ast::LVDStatement::toString(unsigned int indent) const
{
	std::string r(indent, '\t');
	return r + type_ident->toString(indent) + (init_expr ? " = " + init_expr->toString(indent) : "") + ";\n";
}
