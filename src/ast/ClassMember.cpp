#include "ClassMember.hpp"

void ast::ClassMember::setDeclaration(shptr<ClassDeclaration const> decl)
{
	declaration = decl;
}

shptr<ast::ClassDeclaration const> ast::ClassMember::getDeclaration() const
{
	return this->declaration;
}