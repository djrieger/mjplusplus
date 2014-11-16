#include "../globals.hpp"
#include <algorithm>


#include "ClassDeclaration.hpp"
#include "MethodDeclaration.hpp"

namespace ast
{
	ClassDeclaration::ClassDeclaration(uptr<Ident>& class_name, uptr<vec<uptr<ClassMember>>>& members) : class_name(std::move(class_name)), members(std::move(members))
	{
		;
	}

	void ClassDeclaration::toString(std::ostream& out, unsigned int indent, bool) const
	{
		out << std::string(indent, '\t');
		out << "class ";
		class_name->toString(out, indent);
		out << " {\n";

		auto sortMembers = [](const uptr<ast::ClassMember>& a,
		                      const uptr<ast::ClassMember>& b) -> bool
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
