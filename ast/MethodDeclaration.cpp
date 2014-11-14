#include "MethodDeclaration.hpp"

ast::MethodDeclaration::MethodDeclaration(std::unique_ptr<TypeIdent>& return_type_and_name, std::unique_ptr<std::vector<std::unique_ptr<TypeIdent>>>& parameters, std::unique_ptr<Statement>& block)
	: return_type_and_name(std::move(return_type_and_name)), parameters(std::move(parameters)), block(std::move(block))
{
}

std::string ast::MethodDeclaration::toString() const
{
	return toString(false);
}

std::string ast::MethodDeclaration::toString(bool isStatic) const
{
	std::string modifier = isStatic ? "static " : "";
	std::string ret = "public " + modifier + return_type_and_name->toString() + "(";

	for (auto it = parameters->begin(); it != parameters->end(); it++)
	{
		ret += (**it).toString();

		if (it + 1 != parameters->end())
			ret += ", ";
	}

	ret += ")\n" + (block ? block->toString() : "{ }");

	return ret;
}

std::string ast::MethodDeclaration::getName() const
{
	return "M" + return_type_and_name->getName();
}