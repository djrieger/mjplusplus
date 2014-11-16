#include "../globals.hpp"
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

void ast::LVDStatement::toString(std::ostream& out, unsigned int indent, bool) const
{
	out << std::string(indent, '\t');
	type_ident->toString(out, indent);

	if (init_expr)
	{
		out << " = ";
		init_expr->toString(out, indent, true);
	}

	out << ";\n";
}
