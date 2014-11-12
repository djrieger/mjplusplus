#include "MethodDeclaration.hpp"

ast::MethodDeclaration::MethodDeclaration(TypeIdent& return_type_and_name, std::vector<TypeIdent> parameters, Statement& block)
	: return_type_and_name(return_type_and_name), parameters(parameters), block(block)
{

}

std::string ast::MethodDeclaration::toString()
{
	std::string ret = "public " + return_type_and_name.toString() + "(";
	auto it = parameters.begin();

	while (1)
	{
		ret += it.toString();

		if (++it != parameters.end())
			ret += ", ";
		else
			break;
	}

	ret += ")" + std::endl + block.toString();

	return ret;
}

