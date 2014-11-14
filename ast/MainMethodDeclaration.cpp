#include "MainMethodDeclaration.hpp"
#include "TypeIdent.hpp"


ast::MainMethodDeclaration::MainMethodDeclaration(std::unique_ptr<TypeIdent>& return_type_and_name, std::unique_ptr<std::vector<std::unique_ptr<TypeIdent>>>& parameters, std::unique_ptr<Statement>& block)
	: MethodDeclaration(return_type_and_name, parameters, block)
{
	;
}

std::string ast::MainMethodDeclaration::toString(unsigned int indent) const
{
	std::string ret(indent, '\t');
	ret += "public static " + return_type_and_name->toString(indent) + "(";

	for (auto it = parameters->begin(); it != parameters->end(); it++)
	{
		ret += (**it).toString(indent);

		if (it + 1 != parameters->end())
			ret += ", ";
	}

	ret += ") " + (block ? block->toString(indent + 1) : "{ }");

	return ret;
}


