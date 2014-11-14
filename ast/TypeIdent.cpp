#include "TypeIdent.hpp"

ast::TypeIdent::TypeIdent(std::unique_ptr<Type>& type, std::unique_ptr<Ident>& identifier) :
	type(std::move(type)), identifier(std::move(identifier))
{
}

std::string ast::TypeIdent::toString(unsigned int indent) const
{
	return type->toString(indent) + " " + identifier->toString(indent);
}


