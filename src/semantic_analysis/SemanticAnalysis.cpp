#include "SemanticAnalysis.hpp"

SemanticAnalysis::SemanticAnalysis(shptr<ast::Program> program): valid(true), root(program), symboltable(), classTable() {}

void SemanticAnalysis::printError(std::string s)
{
	valid = false;
	std::cout << s << std::endl;
}

bool SemanticAnalysis::start()
{
	root->collectDefinitions(std::make_shared<SemanticAnalysis>(*this));
	return valid;
}

bool SemanticAnalysis::insertClass(const std::string& className, shptr<ast::ClassDeclaration>& node)
{
	return classTable.insert(std::make_pair<const std::string&, shptr<ast::ClassDeclaration>&>(className, node)).second;
}

shptr<SymbolTable> SemanticAnalysis::getSymbolTable() const
{
	return this->symboltable;
}