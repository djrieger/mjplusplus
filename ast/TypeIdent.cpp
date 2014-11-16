#include "TypeIdent.hpp"

ast::TypeIdent::TypeIdent(std::unique_ptr<Type>& type, std::unique_ptr<Ident>& identifier) :
	type(std::move(type)), identifier(std::move(identifier))
{
}

void ast::TypeIdent::toString(std::ostream& out, unsigned int indent, bool special) const
{
	type->toString(out, indent);
	out << " ";
	identifier->toString(out, indent);
}

std::string ast::TypeIdent::getName() const
{
	return identifier->getName();
}

