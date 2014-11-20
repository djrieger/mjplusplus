#include "../globals.hpp"
#include <algorithm>


#include "ClassDeclaration.hpp"
#include "MethodDeclaration.hpp"

namespace ast
{
	ClassDeclaration::ClassDeclaration(shptr<Ident> class_name, shptr<vec<shptr<ClassMember> > > members) : class_name(class_name), members(members)
	{
		;
	}

	void ClassDeclaration::toString(std::ostream& out, unsigned int indent, bool) const
	{
		out << std::string(indent, '\t');
		out << "class ";
		class_name->toString(out, indent);
		out << " {\n";

		auto sortMembers = [](const shptr<ast::ClassMember>& a,
		                      const shptr<ast::ClassMember>& b) -> bool
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

void ClassDeclaration::collectDefinitions(shptr<SemanticAnalysis> sa, shptr<SymbolTable> symbolTable) const
{
	for (auto& classMemberNode : *members) 
	{
		classMemberNode->collectDefinition(sa, symbolTable);
	}
}