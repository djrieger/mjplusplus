#include "BoolLiteral.hpp"

namespace ast
{
	BoolLiteral::BoolLiteral(bool value) : value(value)
	{
		;
	}

	std::string BoolLiteral::toString()
	{
		return value ? "true" : "false";
	}
}
