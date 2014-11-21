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

void ast::FieldDeclaration::collectDefinition(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	auto type = type_and_name->getType();
	auto primitiveType = type->getPrimitiveType();

	if (primitiveType == Type::Primitive_type::VOID)
		sa.printError("Field " + type_and_name->getName() + " cannot have type void.");
	else if (primitiveType == Type::Primitive_type::NONE)
	{
		// We have a reference type. Find corresponding class in class table:
		auto iter = sa.getClassTable().find(type->getClassName());

		if (iter == sa.getClassTable().end())
			sa.printError("Type " + type->getClassName() + " undeclared.");
		else
		{
			auto s = std::make_shared<Symbol>(type_and_name->getName(), symbolTable->getCurrentScope());
			auto d = std::make_shared<Definition>(s, type);
			s->setCurrentDefinition(d);
			symbolTable->insert(s, d);
		}

	}
}