#include "../globals.hpp"
#include "FieldDeclaration.hpp"

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

void ast::FieldDeclaration::collectDefinition(shptr<SemanticAnalysis> sa, shptr<SymbolTable> symbolTable) const
{
	auto primitiveType = type_and_name->getType()->getPrimitiveType();
	if (primitiveType == Type::Primitive_type::VOID)
	{
		sa->printError("Field " + type_and_name->getName() + " cannot have type void.");
	} else if (primitiveType == Type::Primitive_type::NONE) {
		auto iter = sa->getClassTable.find(type_and_name->getType()->getClassName());
		if (iter == sa->getClassTable.end())
		{
			sa->printError("Type " + type_and_name->getType()->getClassName() + " undeclared.");
		} else {
			/*
			Symbol s(&type_and_name->getType()->getClassName(), symbolTable->getCurrentScope(), );
			Definition d()
			s.setDefinition(d);
			symbolTable->insert();
			*/
		}
	}
}