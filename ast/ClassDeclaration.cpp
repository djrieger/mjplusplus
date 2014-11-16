#include <algorithm>


#include "ClassDeclaration.hpp"
#include "MethodDeclaration.hpp"

namespace ast
{
	ClassDeclaration::ClassDeclaration(std::unique_ptr<Ident>& class_name, std::unique_ptr<std::vector<std::unique_ptr<ClassMember>>>& members) : class_name(std::move(class_name)), members(std::move(members))
	{
		;
	}

	void ClassDeclaration::toString(std::ostream& out, unsigned int indent, bool special) const
	{
		out << std::string(indent, '\t');
		out << "class ";
		class_name->toString(out, indent);
		out << " {\n";

		auto sortMembers = [](const std::unique_ptr<ast::ClassMember>& a,
		                      const std::unique_ptr<ast::ClassMember>& b) -> bool
		{
			return a->getName() < b->getName();
		};
		std::sort(members->begin(), members->end(), sortMembers);

		for (auto& it : *members)
			it->toString(out, indent + 1);

		out << "}\n";
	}

	std::string ClassDeclaration::getName() const
	{
		return class_name->getName();
	}
}
