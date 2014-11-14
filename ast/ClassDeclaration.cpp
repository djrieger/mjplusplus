#include "ClassDeclaration.hpp"

namespace ast
{
	ClassDeclaration::ClassDeclaration(std::unique_ptr<Ident>& class_name, std::unique_ptr<std::vector<std::unique_ptr<ClassMember>>>& members) : class_name(std::move(class_name)), members(std::move(members))
	{
		;
	}
	std::string ClassDeclaration::toString(unsigned int indent) const
	{
		std::string r(indent, '\t');
		r += "class " + class_name->toString(indent) + " {\n";

		for (auto& it : *members)
			r += it->toString(indent + 1);

		r += '}';

		return r;
	}
}
