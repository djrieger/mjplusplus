#include "InitLVDStatement.hpp"

namespace ast
{
	InitLVDStatement::InitLVDStatement(TypeIdent& type_ident, Expression& init_expr) : type_ident(type_ident), init_expr(init_expr)
	{
		;
	}

	std::string InitLVDStatement::toString()
	{
		return type_ident.toSting() + " = " + init_expr.toString();
	}
}
