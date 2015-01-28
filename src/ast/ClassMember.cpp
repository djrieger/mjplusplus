#include "ClassMember.hpp"
#include "../firm_interface/FirmInterface.hpp"

void ast::ClassMember::setDeclaration(shptr<ClassDeclaration const> decl)
{
	declaration = decl;
}

shptr<ast::ClassDeclaration const> ast::ClassMember::getDeclaration() const
{
	return this->declaration;
}

std::string ast::ClassMember::mangle() const
{
	return firm::FirmInterface::replaceDollarAndUnderscores(declaration->getName());
}
