#include "MethodDeclaration.hpp"

ast::MethodDeclaration::MethodDeclaration(std::unique_ptr<TypeIdent>& return_type_and_name, std::unique_ptr<std::vector<TypeIdent>>& parameters, std::unique_ptr<Statement>& block)
	: return_type_and_name(std::move(return_type_and_name)), parameters(std::move(parameters)), block(std::move(block))
{

}

std::string ast::MethodDeclaration::toString()
{
	std::string ret = "public " + return_type_and_name->toString() + "(";
	auto it = parameters->begin();

	while (1)
	{
		ret += it->toString();

		if (++it != parameters->end())
			ret += ", ";
		else
			break;
	}

	ret += ")\n" + block->toString();

	return ret;
}
