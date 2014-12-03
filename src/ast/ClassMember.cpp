#include "ClassMember.hpp"

void ast::ClassMember::setDeclaration(shptr<ClassDeclaration const> decl)
{
	declaration = decl;
}
