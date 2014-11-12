#include "FieldDeclaration.hpp"

ast::FieldDeclaration::FieldDeclaration(TypeIdent& type_and_name) : type_and_name(type_and_name)
{

}

std::string ast::FieldDeclaration::toString()
{
	return type_and_name.toString();
}