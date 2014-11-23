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



void ast::MainMethodDeclaration::collectDefinition(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, std::string const&) const
{
	auto symbol = Symbol::makeSymbol(this->getName(), shptr<Scope>());

	if (return_type_and_name->getName() != "main")
		sa.printError("Main method has name \033[1m" + return_type_and_name->getName() + "\033[0m instead of 'main'.", return_type_and_name->getIdent());

	// check if a method with the same name already exists
	if (symbolTable->definedInCurrentScope(symbol))
		sa.printError("Method with name \033[1m" + return_type_and_name->getName() + "\033[0m already declared.", return_type_and_name->getIdent());

	auto returnType = return_type_and_name->getType();//type is void

	symbolTable->enterScope();
	// foo is not necessary as the main method is not inserted into the method table
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
	auto paramSymbol = Symbol::makeSymbol(parameter->getName(), shptr<Scope>());
	auto paramDefinition = std::make_shared<Definition>(paramSymbol, parameter->getType());
	symbolTable->insert(paramSymbol, paramDefinition);
	auto params_type = std::make_shared<vec<shptr<ast::Type>>>();
	params_type->push_back(parameter->getType());
	return params_type;
}

void ast::MainMethodDeclaration::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	auto st = std::make_shared<SymbolTable>(*symbolTable);

	st->leaveScope(); /* remove this from table */
	st->enterScope();
	auto s = Symbol::makeSymbol("return", st->getCurrentScope());
	auto d = std::make_shared<Definition>(s, return_type_and_name->getType());
	st->insert(s, d);

	auto system_s = Symbol::makeSymbol("System", st->getCurrentScope());

	if (!symbolTable->definedInCurrentScope(system_s))
	{
		auto system_t = std::make_shared<ast::Type>(sa.getClassTable().at("#System").classNode->getIdent());
		auto system_d = std::make_shared<Definition>(system_s, system_t);
		st->insert(system_s, system_d);
	}

	if (!block)
	{
		if (*return_type_and_name->getType() != Type(Type::Primitive_type::VOID))
			sa.printError("Method " + return_type_and_name->getName() + " returns non-void but body is empty", return_type_and_name->getIdent());
	}
	else if (!block->analyze(sa, st) && *return_type_and_name->getType() != Type(Type::Primitive_type::VOID))
		sa.printError("Method " + return_type_and_name->getName() + " returns non-void but not all paths return", return_type_and_name->getIdent());

	st->leaveScope();
	st->enterScope(); /* fix scope */
}
