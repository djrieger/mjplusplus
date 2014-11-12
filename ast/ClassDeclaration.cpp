#include "ClassDeclaration.hpp"

namespace ast
{
	ClassDeclaration::ClassDeclaration(Ident& class_name, std::vector<ClassMember> members) : class_name(class_name), members(members)
	{
		;
	}
	std::string ClassDeclaration::toString()
	{
		std::string r("class " + class_name.toString() + "\n{");

		for (auto it = members.begin(); it != members.end(); it++)
			r += it->toString();

		r += '}';
	}
}
