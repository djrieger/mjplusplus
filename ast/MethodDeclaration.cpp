#include "MethodDeclaration.hpp"

ast::MethodDeclaration::MethodDeclaration(std::unique_ptr<TypeIdent>& return_type_and_name, std::unique_ptr<std::vector<std::unique_ptr<TypeIdent>>>& parameters, std::unique_ptr<Statement>& block)
	: return_type_and_name(std::move(return_type_and_name)), parameters(std::move(parameters)), block(std::move(block))
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
		block->toString(out, indent + 1);
	else
		out << "{ }";
}

std::string ast::MethodDeclaration::getName() const
{
	return "M" + return_type_and_name->getName();
}
