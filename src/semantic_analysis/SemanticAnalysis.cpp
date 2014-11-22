#include "SemanticAnalysis.hpp"
#include "../ast/Program.hpp"
#include "../util/ErrorReporter.hpp"

SemanticAnalysis::SemanticAnalysis(shptr<ast::Program> program, shptr<ErrorReporter> errorReporter): errorReporter(errorReporter), valid(true), root(program), symboltable(), classTable() {}

// TODO:
// Remove this method as soon as possible
void SemanticAnalysis::printError(std::string s)
{
	valid = false;
	errorReporter->recordError(ErrorReporter::ErrorType::SEMANTIC, s, std::pair<unsigned int, unsigned int>(1, 1));
}

void SemanticAnalysis::printError(std::string s, shptr<ast::PositionAwareNode> node)
{
	valid = false;
	errorReporter->recordError(ErrorReporter::ErrorType::SEMANTIC, s, node->getPosition());
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
			printError("Type " + type->getClassName() + " undeclared.", type->getClassNameIdent());
			return false;
		}

		return true;
	}

	// Primary type
	return true;
}
