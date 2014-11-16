#include "MainMethodDeclaration.hpp"
#include "TypeIdent.hpp"


ast::MainMethodDeclaration::MainMethodDeclaration(std::unique_ptr<TypeIdent>& return_type_and_name, std::unique_ptr<std::vector<std::unique_ptr<TypeIdent>>>& parameters, std::unique_ptr<Statement>& block)
	: MethodDeclaration(return_type_and_name, parameters, block)
{
	;
}

void ast::MainMethodDeclaration::toString(std::ostream& out, unsigned int indent, bool) const
{
	// Prints this method including the "static" modifier
	MethodDeclaration::toString(out, indent, true);
}
