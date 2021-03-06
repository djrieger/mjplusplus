#include <algorithm>

#include "../globals.hpp"

#include "ClassDeclaration.hpp"
#include "MethodDeclaration.hpp"
#include "MainMethodDeclaration.hpp"
#include "Type.hpp"
#include "../semantic_analysis/symbol_table/Symbol.hpp"

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
			return a->getNameForSort() < b->getNameForSort();
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

	bool ClassDeclaration::collectDefinitions(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable) const
	{
		bool containsMainMethod = false;

		for (auto& classMemberNode : *members)
		{
			if (dynamic_cast<MainMethodDeclaration*>(classMemberNode.get()))
				containsMainMethod = true;

			classMemberNode->setDeclaration(std::static_pointer_cast<ClassDeclaration const>(shared_from_this()));
			classMemberNode->collectDefinition(sa, symbolTable, class_name->getName());

		}

		return containsMainMethod;
	}

	void ClassDeclaration::analyze(semantic::SemanticAnalysis& sa) const
	{
		auto symbolTable = std::make_shared<semantic::symbol::SymbolTable>();
		symbolTable->enterScope();

		for (auto& classMemberNode : *members)
			classMemberNode->analyze(sa, symbolTable);

		symbolTable->leaveScope();
	}

	shptr<Ident> ClassDeclaration::getIdent() const
	{
		return this->class_name;
	}

	void ClassDeclaration::accept(ASTVisitor& visitor) const
	{
		visitor.visit(std::static_pointer_cast<ClassDeclaration const>(shared_from_this()));
	}

	shptr<vec<shptr<ClassMember>>> ClassDeclaration::getMembers() const
	{
		return members;
	}
}
