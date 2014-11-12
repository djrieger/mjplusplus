#include "Ident.hpp"

ast::Ident::Ident(std::string string_value) : string_value(string_value)
{

}

std::string ast::Ident::toString()
{
	return string_value;
}