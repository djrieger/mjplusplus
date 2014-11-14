#include "ClassDeclaration.hpp"

namespace ast
{
	ClassDeclaration::ClassDeclaration(std::unique_ptr<Ident>& class_name, std::unique_ptr<std::vector<std::unique_ptr<ClassMember>>>& members) : class_name(std::move(class_name)), members(std::move(members))
	{
		;
	}
	std::string ClassDeclaration::toString() const
	{
		std::string r("class " + class_name->toString() + " {\n");

		for (auto& it : *members)
			r += it->toString();

		r += '\n';
		r += '}';

		return r;
	}
}
