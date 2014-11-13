#include "ClassDeclaration.hpp"

namespace ast
{
	ClassDeclaration::ClassDeclaration(std::unique_ptr<Ident>& class_name, std::unique_ptr<std::vector<ClassMember>>& members) : class_name(std::move(class_name)), members(std::move(members))
	{
		;
	}
	std::string ClassDeclaration::toString()
	{
		std::string r("class " + class_name->toString() + "\n{");

		for (auto it = members->begin(); it != members->end(); it++)
			r += it->toString();

		r += '}';

		return r;
	}
}
