#include "InitLVDStatement.hpp"

namespace ast
{
	InitLVDStatement::InitLVDStatement(TypeIdent& type_ident, Expression& init_expr) :
		LVDStatement(type_ident), init_expr(init_expr)
	{
		;
	}

	std::string InitLVDStatement::toString()
	{
		return type_ident.toString() + " = " + init_expr.toString();
	}
}
