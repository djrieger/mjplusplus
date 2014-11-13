#include "MainMethodDeclaration.hpp"
#include "TypeIdent.hpp"


ast::MainMethodDeclaration::MainMethodDeclaration(std::unique_ptr<TypeIdent>& return_type_and_name, std::unique_ptr<std::vector<TypeIdent>>& parameters, std::unique_ptr<Statement>& block)
	: MethodDeclaration(return_type_and_name, parameters, block)
{
	;
}

std::string ast::MainMethodDeclaration::toString()
{
	std::string ret = "public static " + return_type_and_name->toString() + "(";
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


