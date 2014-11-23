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

void ast::MethodDeclaration::collectDefinition(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, std::string const& class_name) const
{
	auto symbol = Symbol::makeSymbol(this->getName(), shptr<Scope>());

	// check if a method with the same name already exists
	if (symbolTable->definedInCurrentScope(symbol))
		sa.printError("Method with name \033[1m" + return_type_and_name->getName() + "\033[0m already declared.", return_type_and_name->getIdent());

	auto returnType = return_type_and_name->getType();
	auto primitiveType = returnType->getPrimitiveType();

	// We have a reference type. Find corresponding class in class table:
	if (primitiveType == Type::Primitive_type::NONE && sa.getClassTable().find(returnType->getClassName()) == sa.getClassTable().end())
		sa.printError("Type \033[1m" + returnType->getClassName() + "\033[0m used as return type undeclared.", returnType->getClassNameIdent());

	if (primitiveType == Type::Primitive_type::VOID&&  returnType->getDimension() > 0)
		sa.printError("Cannot have an array with void as base type.");

	symbolTable->enterScope();
	auto param_types = collectParameters(sa, symbolTable);
	symbolTable->leaveScope();

	// insert this method into symbol table of this class
	auto definition = std::make_shared<Definition>(symbol, returnType);
	symbolTable->insert(symbol, definition);

	// insert this method into the method table in the class table
	auto ct = sa.getClassTable();
	//shptr<MethodDeclaration> md_node;
	//md_node.reset(this->enable_shared_from_this());
	auto const foo = shared_from_this();
	auto md_node = std::static_pointer_cast<const MethodDeclaration>(foo);
	//	const std::shared_ptr<MethodDeclaration> md_node = this; //auto md_node = std::make_shared<MethodDeclaration>(this);
	//md_node.reset(this);
	ct[class_name].methodTable->insertMethod(this->getName(), md_node, returnType, param_types);
}

shptr<vec<shptr<ast::Type>>> ast::MethodDeclaration::collectParameters(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	auto param_types = std::make_shared<vec<shptr<ast::Type>>>();

	for (auto& parameter : *parameters)
	{
		auto paramSymbol = Symbol::makeSymbol("p" + parameter->getName(), shptr<Scope>());

		if (symbolTable->definedInCurrentScope(paramSymbol))
			sa.printError("Parameter with name \033[1m" + parameter->getName() + "\033[0m already declared in this function.", parameter->getIdent());

		Type::Primitive_type primitiveType = parameter->getType()->getPrimitiveType();
		param_types->push_back(parameter->getType());

		// check parameters for type void
		if (primitiveType == Type::Primitive_type::VOID)
			sa.printError("Parameter \033[1m" + parameter->getName() + "\033[0m cannot have type void.", parameter->getIdent());
		// check if referenced paramater type exists:
		else if (primitiveType == Type::Primitive_type::NONE && sa.getClassTable().find(parameter->getType()->getClassName()) == sa.getClassTable().end())
			sa.printError("Type \033[1m" + parameter->getType()->getClassName() + "\033[0m of parameter \033[1m" + parameter->getName() + "\033[0m undeclared.", parameter->getType()->getClassNameIdent());

		auto paramDefinition = std::make_shared<Definition>(paramSymbol, parameter->getType());
		symbolTable->insert(paramSymbol, paramDefinition);
	}

	return param_types;
}

void ast::MethodDeclaration::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	auto st = std::make_shared<SymbolTable>(*symbolTable);

	st->enterScope();
	auto s = Symbol::makeSymbol("return", st->getCurrentScope());
	auto d = std::make_shared<Definition>(s, return_type_and_name->getType());
	st->insert(s, d);

	if (!block)
	{
		if (*return_type_and_name->getType() != Type(Type::Primitive_type::VOID))
			sa.printError("Method " + return_type_and_name->getName() + " returns non-void but body is empty", return_type_and_name->getIdent());
	}
	else if (!block->analyze(sa, st) && *return_type_and_name->getType() != Type(Type::Primitive_type::VOID))
		sa.printError("Method " + return_type_and_name->getName() + " returns non-void but not all paths return", return_type_and_name->getIdent());

	st->leaveScope();
}
