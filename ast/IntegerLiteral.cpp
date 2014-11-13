#include "IntegerLiteral.hpp"


ast::IntegerLiteral::IntegerLiteral(std::string const& string_value)
	: string_value(string_value)
{

}

std::string ast::IntegerLiteral::toString()
{
	return string_value;
}
