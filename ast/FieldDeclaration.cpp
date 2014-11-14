#include "FieldDeclaration.hpp"

ast::FieldDeclaration::FieldDeclaration(std::unique_ptr<TypeIdent>& type_and_name) : type_and_name(std::move(type_and_name))
{

}

std::string ast::FieldDeclaration::toString(unsigned int indent) const
{
	std::string r(indent, '\t');
	return r + "public " + type_and_name->toString(indent) + ";\n";
}
