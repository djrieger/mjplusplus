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



void ast::MainMethodDeclaration::collectDefinition(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, std::string const& class_name) const
{
	auto symbol = Symbol::makeSymbol(this->getName(), shptr<Scope>());

	if (return_type_and_name->getName() != "main")
		sa.printError("Main method has name \033[1m" + return_type_and_name->getName() + "\033[0m instead of 'main'.", return_type_and_name->getIdent());

	// check if a method with the same name already exists
	if (symbolTable->definedInCurrentScope(symbol))
		sa.printError("Method with name \033[1m" + return_type_and_name->getName() + "\033[0m already declared.", return_type_and_name->getIdent());

	auto returnType = return_type_and_name->getType();//type is void

	symbolTable->enterScope();
	auto foo = collectParameters(sa, symbolTable);
	symbolTable->leaveScope();

	// insert this field into symbol table of this class
	auto definition = std::make_shared<Definition>(symbol, returnType);
	symbolTable->insert(symbol, definition);
}

shptr<vec<shptr<ast::Type>>> ast::MainMethodDeclaration::collectParameters(SemanticAnalysis&, shptr<SymbolTable> symbolTable) const
{
	//we know we only have one parameter, which is of a pseudo-type String[]
	auto parameter = (*parameters)[0];
	auto paramSymbol = Symbol::makeSymbol("p" + parameter->getName(), shptr<Scope>());
	auto paramDefinition = std::make_shared<Definition>(paramSymbol, parameter->getType());
	symbolTable->insert(paramSymbol, paramDefinition);
	auto params_type = std::make_shared<vec<shptr<ast::Type>>>();
	params_type->push_back(parameter->getType());
	return params_type;
}
