#include "../globals.hpp"
#include "MainMethodDeclaration.hpp"
#include "TypeIdent.hpp"


ast::MainMethodDeclaration::MainMethodDeclaration(shptr<TypeIdent> return_type_and_name, shptr<vec<shptr<TypeIdent> > > parameters, shptr<Statement> block)
	: MethodDeclaration(return_type_and_name, parameters, block)
{
	;
}

void ast::MainMethodDeclaration::toString(std::ostream& out, unsigned int indent, bool) const
{
	// Prints this method including the "static" modifier
	MethodDeclaration::toString(out, indent, true);
}

void ast::MainMethodDeclaration::collectParameters(SemanticAnalysis&, shptr<SymbolTable> symbolTable) const
{
	//we know we only have one parameter, which is of a pseudo-type String[]
	auto parameter = (*parameters)[0];
	auto paramSymbol = Symbol::makeSymbol("p" + parameter->getName(), shptr<Scope>());
	auto paramDefinition = std::make_shared<Definition>(paramSymbol, parameter->getType());
	symbolTable->insert(paramSymbol, paramDefinition);
}
