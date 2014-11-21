#include "../globals.hpp"
#include <algorithm>

#include "Program.hpp"
#include "../semantic_analysis/SemanticAnalysis.hpp"

ast::Program::Program(shptr<vec<shptr<ClassDeclaration>>> classes) : classes(classes)
{

}

void ast::Program::toString(std::ostream& out, unsigned int indent, bool) const
{
	auto sortClasses = [](const shptr<ast::ClassDeclaration>& a,
	                      const shptr<ast::ClassDeclaration>& b) -> bool
	{
		return a->getName() < b->getName();
	};
	std::sort(classes->begin(), classes->end(), sortClasses);

	for (auto& c : *classes)
		c->toString(out, indent);
}

void ast::Program::collectDefinitions(SemanticAnalysis& sa) const
{
	for (auto& classDeclNode : *classes)
	{
		if (!sa.insertClass(classDeclNode->getName(), classDeclNode))
			sa.printError("Class with name \033[1m" + classDeclNode->getName() + "\033[0m already defined.");
	}

	for (auto& item : sa.getClassTable())
		item.second.classNode->collectDefinitions(sa, item.second.symbolTable);

	for (auto& item : sa.getClassTable())
		item.second.classNode->analyze(sa, item.second.symbolTable);

}
