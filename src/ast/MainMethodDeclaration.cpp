#include "../globals.hpp"
#include "MainMethodDeclaration.hpp"
#include "TypeIdent.hpp"


ast::MainMethodDeclaration::MainMethodDeclaration(shptr<TypeIdent> return_type_and_name, shptr<vec<shptr<TypeIdent> > > parameters, shptr<Statement> block)
	: MethodDeclaration(std::move(return_type_and_name), std::move(parameters), std::move(block))
{
	;
}

void ast::MainMethodDeclaration::toString(std::ostream& out, unsigned int indent, bool) const
{
	// Prints this method including the "static" modifier
	MethodDeclaration::toString(out, indent, true);
}
