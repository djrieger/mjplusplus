#include "MethodDeclaration.hpp"

ast::MethodDeclaration::MethodDeclaration(std::unique_ptr<TypeIdent>& return_type_and_name, std::unique_ptr<std::vector<std::unique_ptr<TypeIdent>>>& parameters, std::unique_ptr<Statement>& block)
	: return_type_and_name(std::move(return_type_and_name)), parameters(std::move(parameters)), block(std::move(block))
{

}

std::string ast::MethodDeclaration::toString(unsigned int indent) const
{
	std::string ret = std::string(indent, '\t') + "public " + return_type_and_name->toString(indent) + "(";

	for (auto it = parameters->begin(); it != parameters->end(); it++)
	{
		ret += (**it).toString(indent);

		if (it + 1 != parameters->end())
			ret += ", ";
	}

	ret += ") " + (block ? block->toString(indent + 1) : "{ }");

	return ret;
}
