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

shptr<ast::Ident> const& ast::TypeIdent::getIdent() const
{
	return identifier;
}

shptr<ast::Type> const& ast::TypeIdent::getType() const
{
	return type;
}

shptr<ast::Type> const& ast::TypeIdent::getDeclType() const
{
	return type;
}

void ast::TypeIdent::accept(ASTVisitor& visitor) const
{
	visitor.visit(std::static_pointer_cast<TypeIdent const>(shared_from_this()));
}
