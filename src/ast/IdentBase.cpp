#include "IdentBase.hpp"

ast::IdentBase::IdentBase()
{
}

shptr<ast::VariableDeclaration const> ast::IdentBase::getDeclaration() const
{
	return declaration;
}