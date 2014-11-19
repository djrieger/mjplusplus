#include "../globals.hpp"
#include "FieldDeclaration.hpp"

ast::FieldDeclaration::FieldDeclaration(uptr<TypeIdent> type_and_name) : type_and_name(std::move(type_and_name))
{

}

void ast::FieldDeclaration::toString(std::ostream& out, unsigned int indent, bool) const
{
	out << std::string(indent, '\t');
	out << "public ";
	type_and_name->toString(out, indent);
	out << ";\n";
}

std::string ast::FieldDeclaration::getName() const
{
	return "f" + type_and_name->getName();
}
