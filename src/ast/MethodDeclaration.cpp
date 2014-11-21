#include "../globals.hpp"
#include "MethodDeclaration.hpp"

ast::MethodDeclaration::MethodDeclaration(shptr<TypeIdent> return_type_and_name, shptr<vec<shptr<TypeIdent>>> parameters, shptr<Statement> block)
	: return_type_and_name(return_type_and_name), parameters(parameters), block(block)
{
}

void ast::MethodDeclaration::toString(std::ostream& out, unsigned int indent, bool isStatic) const
{
	std::string modifier = isStatic ? "static " : "";
	out << std::string(indent, '\t') << "public " << modifier;
	return_type_and_name->toString(out, indent);
	out << "(";

	for (auto it = parameters->begin(); it != parameters->end(); it++)
	{
		(**it).toString(out, indent);

		if (it + 1 != parameters->end())
			out << ", ";
	}

	out << ") ";

	if (block)
		block->toString(out, indent, true);
	else
		out << "{ }\n";
}

std::string ast::MethodDeclaration::getName() const
{
	return "M" + return_type_and_name->getName();
}

void ast::MethodDeclaration::collectDefinition(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	/*
	auto primitiveType = return_type_and_name->getType()->getPrimitiveType();

	if (primitiveType == Type::Primitive_type::VOID)
		sa.printError("Field " + type_and_name->getName() + " cannot have type void.");
	else if (primitiveType == Type::Primitive_type::NONE)
	{
		/*
		auto iter = sa.getClassTable.find(type_and_name->getType()->getClassName());

		if (iter == sa.getClassTable.end())
			sa.printError("Type " + type_and_name->getType()->getClassName() + " undeclared.");
		else
		{
			/*
			Symbol s(&type_and_name->getType()->getClassName(), symbolTable->getCurrentScope(), );
			Definition d()
			s.setDefinition(d);
			symbolTable->insert();
			*/
	//}

	//}
}

void ast::MethodDeclaration::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symboltable) const
{
	symboltable->enterScope();
	auto s = Symbol::makeSymbol("return", symboltable->getCurrentScope());
	auto d = std::make_shared<Definition>(s, return_type_and_name->getType());
	symboltable->insert(s, d);

	block->analyze(sa, symboltable);
	symboltable->leaveScope();
}
