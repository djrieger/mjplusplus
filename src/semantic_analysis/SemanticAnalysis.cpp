#include "SemanticAnalysis.hpp"
#include "../ast/Program.hpp"
#include "../util/ErrorReporter.hpp"

SemanticAnalysis::SemanticAnalysis(shptr<ast::Program> program): valid(true), root(program), symboltable(), classTable() {}

void SemanticAnalysis::printError(std::string s)
{
	valid = false;
	std::cerr << "\033[1;31mSemantic error:\033[0m " << s << std::endl;

}

void SemanticAnalysis::printError(std::string s, shptr<ast::PositionAwareNode> node)
{
	valid = false;
	reporter.printError(ErrorReporter::ErrorType::SEMANTIC, s, node->getPosition());
}

bool SemanticAnalysis::start()
{
	root->collectDefinitions(*this);
	return valid;
}

bool SemanticAnalysis::insertClass(const std::string& className, shptr<ast::ClassDeclaration>& node)
{
	return classTable.insert({className, {node, std::make_shared<SymbolTable>()}}).second;
}

shptr<SymbolTable> SemanticAnalysis::getSymbolTable() const
{
	return this->symboltable;
}

std::unordered_map<std::string, SemanticAnalysis::ClassTableItem> const& SemanticAnalysis::getClassTable() const
{
	return this->classTable;
}

bool SemanticAnalysis::isTypeDefined(shptr<ast::Type> type, bool isVoidAcceptable)
{
	auto primitiveType = type->getPrimitiveType();

	if (primitiveType == ast::Type::Primitive_type::VOID)
		return isVoidAcceptable;
	else if (primitiveType == ast::Type::Primitive_type::NONE)
	{
		// We have a reference type. Find corresponding class in class table:
		auto iter = getClassTable().find(type->getClassName());

		// not in class table:
		if (iter == getClassTable().end())
		{
			printError("Type " + type->getClassName() + " undeclared.");
			return false;
		}

		return true;
	}

	// Primary type
	return true;
}
