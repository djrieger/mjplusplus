#include "../globals.hpp"
#include <algorithm>

#include "Program.hpp"
#include "../semantic_analysis/SemanticAnalysis.hpp"

ast::Program::Program(uptr<vec<uptr<ClassDeclaration>>> classes) : classes(std::move(classes))
{

}

void ast::Program::toString(std::ostream& out, unsigned int indent, bool) const
{
	//TODO: sort
	auto sortClasses = [](const uptr<ast::ClassDeclaration>& a,
	                      const uptr<ast::ClassDeclaration>& b) -> bool
	{
		return a->getName() < b->getName();
	};
	std::sort(classes->begin(), classes->end(), sortClasses);

	for (auto& c : *classes)
		c->toString(out, indent);
}

void ast::Program::collectDefinitions(shptr<SemanticAnalysis> sa) const
{
	// pseudo
	//sa->getSymbolTable()->enterScope();

	for (auto& _class : *classes)
	{
		shptr<ast::ClassDeclaration> foo(_class.get());

		if (!sa->insertClass(_class->getName(), foo))
			sa->printError("Class with name " + _class->getName() + " already defined.");

		/*
				auto classSymbol = std::make_shared<Symbol>(_class->getName(), symboltable->getCurrentScope());
				auto classDef = std::make_shared<Definition>(classSymbol, _class);
				classSymbol->setDefinition(classDef);
				classesSymbolTable->insert(classSymbol);
		*/
		//symboltable.enterScope();
		//_class.collectDefinitions(symboltable);
		//symboltable.leaveScope();

		// copy class symbol table to list of class symbol tables:

	}

	//sa->getSymbolTable()->leaveScope();
}