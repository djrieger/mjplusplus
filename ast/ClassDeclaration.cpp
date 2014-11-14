#include <algorithm>
#include "ClassDeclaration.hpp"
#include "MethodDeclaration.hpp"

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

		auto sortMembers = [](const std::unique_ptr<ast::ClassMember>& a,
		                      const std::unique_ptr<ast::ClassMember>& b) -> bool
		{
			return a->getName() < b->getName();
		};
		std::sort(members->begin(), members->end(), sortMembers);

		for (auto& it : *members)
			r += it->toString(indent + 1);

		r += '}';

		return r;
	}
}
