#include "../globals.hpp"
#include "TypeIdent.hpp"

ast::TypeIdent::TypeIdent(shptr<Type> type, shptr<Ident> identifier) :
	type(std::move(type)), identifier(std::move(identifier))
{
}

void ast::TypeIdent::toString(std::ostream& out, unsigned int indent, bool) const
{
	type->toString(out, indent);
	out << " ";
	identifier->toString(out, indent);
}

std::string ast::TypeIdent::getName() const
{
	return identifier->getName();
}

