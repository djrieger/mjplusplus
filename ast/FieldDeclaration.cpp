#include "FieldDeclaration.hpp"

ast::FieldDeclaration::FieldDeclaration(std::unique_ptr<TypeIdent>& type_and_name) : type_and_name(std::move(type_and_name))
{

}

std::string ast::FieldDeclaration::toString()
{
	return "public " + type_and_name->toString() + ";\n";
}
