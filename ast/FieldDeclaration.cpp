#include "FieldDeclaration.hpp"

ast::FieldDeclaration::FieldDeclaration(std::unique_ptr<TypeIdent>& type_and_name) : type_and_name(std::move(type_and_name))
{

}

std::string ast::FieldDeclaration::toString() const
{
	return "public " + type_and_name->toString() + ";\n";
}

std::string ast::FieldDeclaration::getName() const
{
	return "f" + type_and_name->getName();
}