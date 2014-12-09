#include "../globals.hpp"
#include "MethodDeclaration.hpp"
#include "../firm_interface/FirmInterface.hpp"

ast::MethodDeclaration::MethodDeclaration(shptr<TypeIdent> return_type_and_name, shptr<vec<shptr<TypeIdent>>> parameters, shptr<Block> block)
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

std::string ast::MethodDeclaration::getNameForSort() const
{
	return "#" + return_type_and_name->getName();
}


std::string ast::MethodDeclaration::getName() const
{
	return return_type_and_name->getName();
}


void ast::MethodDeclaration::collectDefinition(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, std::string const& class_name) const
{
	auto symbol = Symbol::makeSymbol(this->getNameForSort(), shptr<Scope>());

	// check if a method with the same name already exists
	if (symbolTable->definedInCurrentScope(symbol))
		sa.reportError("Method with name $ident{" + return_type_and_name->getName() + "} already declared.", return_type_and_name->getIdent());

	auto returnType = return_type_and_name->getType();
	auto primitiveType = returnType->getPrimitiveType();

	// We have a reference type. Find corresponding class in class table:
	if (primitiveType == Type::Primitive_type::NONE && sa.getClassTable().find(returnType->getClassName()) == sa.getClassTable().end())
		sa.reportError("Type $type{" + returnType->getClassName() + "} used as return type undeclared.", returnType->getClassNameIdent());

	if (primitiveType == Type::Primitive_type::VOID&&  returnType->getDimension() > 0)
		sa.reportError("Cannot have an array with void as base type.", return_type_and_name->getIdent());

	symbolTable->enterScope();
	auto param_types = collectParameters(sa, symbolTable);
	symbolTable->leaveScope();

	// insert this method into symbol table of this class
	auto definition = std::make_shared<Definition>(symbol, nullptr);
	symbolTable->insert(symbol, definition);

	// insert this method into the method table in the class table
	auto ct = sa.getClassTable();
	auto const foo = shared_from_this();
	auto md_node = std::static_pointer_cast<const MethodDeclaration>(foo);
	ct[class_name].methodTable->insertMethod(return_type_and_name->getName(), md_node, returnType, param_types);
}

shptr<vec<shptr<ast::Type>>> ast::MethodDeclaration::collectParameters(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	auto param_types = std::make_shared<vec<shptr<ast::Type>>>();

	for (auto& parameter : *parameters)
	{
		auto paramSymbol = Symbol::makeSymbol(parameter->getName(), shptr<Scope>());

		if (symbolTable->definedInCurrentScope(paramSymbol))
			sa.reportError("Parameter with name $ident{" + parameter->getName() + "} already declared in this function.", parameter->getIdent());

		Type::Primitive_type primitiveType = parameter->getType()->getPrimitiveType();
		param_types->push_back(parameter->getType());

		// check parameters for type void
		if (primitiveType == Type::Primitive_type::VOID)
			sa.reportError("Parameter $ident{" + parameter->getName() + "} cannot have type void.", parameter->getIdent());
		// check if referenced paramater type exists:
		else if (primitiveType == Type::Primitive_type::NONE && sa.getClassTable().find(parameter->getType()->getClassName()) == sa.getClassTable().end())
			sa.reportError("Type $type{" + parameter->getType()->getClassName() + "} of parameter $ident{" + parameter->getName() + "} undeclared.", parameter->getType()->getClassNameIdent());

		auto paramDefinition = std::make_shared<Definition>(paramSymbol, parameter);
		symbolTable->insert(paramSymbol, paramDefinition);
	}

	return param_types;
}

void ast::MethodDeclaration::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	symbolTable->enterScope();
	auto s = Symbol::makeSymbol("return", symbolTable->getCurrentScope());
	auto d = std::make_shared<Definition>(s, return_type_and_name);
	symbolTable->insert(s, d);
	collectParameters(sa, symbolTable);

	auto tt = std::make_shared<TypeIdent>(std::make_shared<Type>(declaration->getIdent()), declaration->getIdent());
	auto ts = Symbol::makeSymbol("this", symbolTable->getCurrentScope());
	auto td = std::make_shared<Definition>(ts, tt);
	symbolTable->insert(ts, td);

	auto system_s = Symbol::makeSymbol("System");

	if (!symbolTable->definedInCurrentScope(system_s))
	{
		auto system_d = std::make_shared<Definition>(system_s, SemanticAnalysis::systemTypeIdent);
		symbolTable->insert(system_s, system_d);
	}

	if (!block)
	{
		if (*return_type_and_name->getType() != Type(Type::Primitive_type::VOID))
			sa.reportError("Method $ident{" + return_type_and_name->getName() + "} returns non-void but body is empty", return_type_and_name->getIdent());
	}
	else if (!block->analyze(sa, symbolTable) && *return_type_and_name->getType() != Type(Type::Primitive_type::VOID))
		sa.reportError("Method $ident{" + return_type_and_name->getName() + "} returns non-void but not all paths return", return_type_and_name->getIdent());

	symbolTable->leaveScope();
}

shptr<vec<shptr<ast::TypeIdent>>> ast::MethodDeclaration::getParameters() const
{
	return parameters;
}

shptr<ast::Type> ast::MethodDeclaration::getReturnType() const
{
	return return_type_and_name->getType();
}

shptr<ast::Block> ast::MethodDeclaration::getBlock() const
{
	return block;
}

unsigned int ast::MethodDeclaration::countVariableDeclarations() const
{
	// Number of method parameters + this parameter + local variables in block.
	// Note that the implementation for MainMethodDeclaration should differ!
	return parameters->size() + 1 + (block ? block->countVariableDeclarations() : 0);
}

void ast::MethodDeclaration::createVariablePositions() const
{

	// If you change anything here, you might want to make changes to
	// MainMethodDeclaration::createVariablePositions(), too!

	auto var2pos = std::make_shared<std::map<std::string, int>>();
	firm::FirmInterface::getInstance().setVarMap(var2pos);
	int pos = 0;

	(*var2pos)["this"] = pos++;

	for (auto parameter : *parameters)
		(*var2pos)[parameter->getName()] = pos++;

	if (block)
		pos = block->setVariablePositions(pos);
}

void ast::MethodDeclaration::accept(ASTVisitor& visitor) const
{
	visitor.visit(std::static_pointer_cast<MethodDeclaration const>(shared_from_this()));
}

std::string ast::MethodDeclaration::mangle() const
{
	return ClassMember::mangle() + "_M" + getName();
}
