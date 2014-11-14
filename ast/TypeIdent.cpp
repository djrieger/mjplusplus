#include "TypeIdent.hpp"

ast::TypeIdent::TypeIdent(std::unique_ptr<Type>& type, std::unique_ptr<Ident>& identifier) :
	type(std::move(type)), identifier(std::move(identifier))
{
}

std::string ast::TypeIdent::toString() const
{
	return type->toString() + " " + identifier->toString();
}

std::string ast::TypeIdent::getName() const
{
	return identifier->getName();
}

