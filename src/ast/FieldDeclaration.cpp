#include "../globals.hpp"
#include "FieldDeclaration.hpp"
#include "../lexer/token.hpp"

ast::FieldDeclaration::FieldDeclaration(shptr<TypeIdent> type_and_name) : type_and_name(type_and_name)
{

}

void ast::FieldDeclaration::toString(std::ostream& out, unsigned int indent, bool) const
{
	out << std::string(indent, '\t');
	out << "public ";
	type_and_name->toString(out, indent);
	out << ";\n";
}

std::string ast::FieldDeclaration::getName() const
{
	return "f" + type_and_name->getName();
}

void ast::FieldDeclaration::collectDefinition(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	auto symbol = Symbol::makeSymbol(this->getName(), shptr<Scope>());

	// check if a field with the same name already exists
	if (symbolTable->definedInCurrentScope(symbol))
		sa.printError("Field with name \033[1m" + type_and_name->getName() + "\033[0m already declared.", type_and_name->getIdent());

	auto type = type_and_name->getType();
	auto primitiveType = type->getPrimitiveType();

	if (primitiveType == Type::Primitive_type::VOID)
		sa.printError("Field " + type_and_name->getName() + " cannot have type void.", type_and_name->getIdent());
	else
	{
		if (primitiveType == Type::Primitive_type::NONE)
		{
			// We have a reference type. Find corresponding class in class table:
			auto iter = sa.getClassTable().find(type->getClassName());

			// not in class table:
			if (iter == sa.getClassTable().end())
				sa.printError("Type " + type->getClassName() + " undeclared.", type->getClassNameIdent());
		}
	}

	auto definition = std::make_shared<Definition>(symbol, type);
	symbolTable->insert(symbol, definition);
}

void ast::FieldDeclaration::analyze(SemanticAnalysis&, shptr<SymbolTable>) const
{
	/* does nothing */
}
