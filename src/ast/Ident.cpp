#include "../globals.hpp"
#include "Ident.hpp"

ast::Ident::Ident(std::string const& string_value) : string_value(string_value)
{

}

void ast::Ident::toString(std::ostream& out, unsigned int, bool) const
{
	out << string_value;
}

std::string ast::Ident::getName() const
{
	return string_value;
}

bool ast::Ident::operator==(ast::Ident const& other)
{
	return &string_value == &other.string_value;
}

bool ast::Ident::operator!=(ast::Ident const& other)
{
	return !(*this == other);
}
