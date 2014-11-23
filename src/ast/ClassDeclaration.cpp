#include <algorithm>

#include "../globals.hpp"

#include "ClassDeclaration.hpp"
#include "MethodDeclaration.hpp"
#include "Type.hpp"
#include "../util/symbol_table/Symbol.hpp"

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

	void ClassDeclaration::collectDefinitions(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
	{
		for (auto& classMemberNode : *members)
			classMemberNode->collectDefinition(sa, symbolTable, class_name->getName());
	}

	void ClassDeclaration::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
	{
		auto t = std::make_shared<Type>(class_name);
		auto s = Symbol::makeSymbol("this", std::make_shared<Scope>(*symbolTable->getCurrentScope()));
		auto d = std::make_shared<Definition>(s, t);
		symbolTable->enterScope();
		symbolTable->insert(s, d);

		for (auto& classMemberNode : *members)
			classMemberNode->analyze(sa, symbolTable);

		symbolTable->leaveScope();
	}

	shptr<Ident> ClassDeclaration::getIdent() const
	{
		return this->class_name;
	}
}
