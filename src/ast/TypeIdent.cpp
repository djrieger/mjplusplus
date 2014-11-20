#include "../globals.hpp"
#include "TypeIdent.hpp"

ast::TypeIdent::TypeIdent(shptr<Type> type, shptr<Ident> identifier) :
	type(type), identifier(identifier)
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

shptr<ast::Type> const &ast::TypeIdent::getType() const
{
	return this->type;
}