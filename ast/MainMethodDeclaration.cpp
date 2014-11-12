#include "MainMethodDeclaration.hpp"
#include "TypeIdent.hpp"


ast::MainMethodDeclaration::MainMethodDeclaration(TypeIdent& return_type_and_name, std::vector<TypeIdent> parameters, Statement& block)
	: MethodDeclaration(return_type_and_name, parameters, block)
{
	;
}

std::string ast::MainMethodDeclaration::toString()
{
	std::string ret = "public static " + return_type_and_name.toString() + "(";
	auto it = parameters.begin();

	while (1)
	{
		ret += it->toString();

		if (++it != parameters.end())
			ret += ", ";
		else
			break;
	}

	ret += ")\n" + block.toString();

	return ret;
}


