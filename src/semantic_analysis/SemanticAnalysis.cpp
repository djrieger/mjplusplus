#include "SemanticAnalysis.hpp"
#include "../ast/Program.hpp"
#include "../ast/TypeIdent.hpp"

#include "../util/ErrorReporter.hpp"

namespace semantic
{

	SemanticAnalysis::SemanticAnalysis(shptr<ast::Program> program, shptr<ErrorReporter> errorReporter): errorReporter(errorReporter), valid(true), root(program), /*symboltable(),*/ classTable() {}

	// TODO:
	// Remove this method as soon as possible

	void SemanticAnalysis::reportError(std::string message)
	{
		valid = false;
		errorReporter->recordError(ErrorReporter::ErrorType::SEMANTIC, message);
	}

	void SemanticAnalysis::reportError(std::string message, shptr<ast::PositionAwareNode> node)
	{
		reportError(message, node->getPosition());
	}

	void SemanticAnalysis::reportError(std::string message, source_position_t position)
	{
		valid = false;
		errorReporter->recordError(ErrorReporter::ErrorType::SEMANTIC, message, position);
	}

	bool SemanticAnalysis::start()
	{
		// add pseudo-classes for System.out.println(int)
		root->addPseudoClasses();

		// start actual checking
		root->collectDefinitions(*this);
		root->analyze(*this);
		return valid;
	}

	bool SemanticAnalysis::insertClass(const std::string& className, shptr<ast::ClassDeclaration>& node)
	{
		return classTable.insert({className, {node, std::make_shared<symbol::SymbolTable>(), std::make_shared<FieldTable>(), std::make_shared<MethodTable>()}}).second;
	}

	std::unordered_map<std::string, SemanticAnalysis::ClassTableItem> const& SemanticAnalysis::getClassTable() const
	{
		return this->classTable;
	}

	bool SemanticAnalysis::isTypeDefined(shptr<ast::Type> type, bool isVoidAcceptable)
	{
		if (!type)
			return false;

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
				reportError("Type $type{" + type->getClassName() + "} undeclared.", type->getClassNameIdent());
				return false;
			}

			return true;
		}

		// Primary type
		return true;
	}

	shptr<ast::TypeIdent> const SemanticAnalysis::systemTypeIdent = std::make_shared<ast::TypeIdent>(
	            std::make_shared<ast::Type>(std::make_shared<ast::Ident>(lexer::Token {lexer::Token::Token_type::TOKEN_IDENT, lexer::Token::getTableReference("$System"), { -1, 0}})),
	            std::make_shared<ast::Ident>(lexer::Token {lexer::Token::Token_type::TOKEN_IDENT, lexer::Token::getTableReference("System"), { -1, 0}})
	        );
}
