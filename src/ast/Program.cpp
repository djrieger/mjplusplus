#include "../globals.hpp"
#include <algorithm>

#include "Program.hpp"
#include "FieldDeclaration.hpp"
#include "MethodDeclaration.hpp"
#include "ClassDeclaration.hpp"
#include "TypeIdent.hpp"

#include "../semantic_analysis/SemanticAnalysis.hpp"

ast::Program::Program(shptr<vec<shptr<ClassDeclaration>>> classes) : classes(classes)
{

}

void ast::Program::toString(std::ostream& out, unsigned int indent, bool) const
{
	auto sortClasses = [](const shptr<ast::ClassDeclaration>& a,
	                      const shptr<ast::ClassDeclaration>& b) -> bool
	{
		return a->getName() < b->getName();
	};
	std::sort(classes->begin(), classes->end(), sortClasses);

	for (auto& c : *classes)
		c->toString(out, indent);
}

void ast::Program::collectDefinitions(SemanticAnalysis& sa) const
{
	for (auto& classDeclNode : *classes)
	{
		if (!sa.insertClass(classDeclNode->getName(), classDeclNode))
			sa.printError("Class with name \033[1m" + classDeclNode->getName() + "\033[0m already defined.", classDeclNode->getIdent());
	}

	for (auto& item : sa.getClassTable())
		item.second.classNode->collectDefinitions(sa, item.second.symbolTable);

	for (auto& item : sa.getClassTable())
		item.second.classNode->analyze(sa, item.second.symbolTable);

}

void ast::Program::addPseudoClasses()
{
	// insert fake classes for void System.out.println(int)

	// void println
	auto prt = std::make_shared<Type>(ast::Type::Primitive_type::VOID);
	lexer::Token prit {lexer::Token::Token_type::TOKEN_IDENT, lexer::Token::getTableReference("println"), {0, 0}};
	auto pri = std::make_shared<Ident>(prit);
	auto pr = std::make_shared<TypeIdent>(prt, pri);
	// int i
	auto pat = std::make_shared<Type>(ast::Type::Primitive_type::INT);
	lexer::Token pait {lexer::Token::Token_type::TOKEN_IDENT, lexer::Token::getTableReference("i"), {0, 0}};
	auto pai = std::make_shared<Ident>(pait);
	auto pa = std::make_shared<std::vector<shptr<TypeIdent>>>();
	pa->push_back(std::make_shared<TypeIdent>(pat, pai));
	// void println(int i)
	auto p = std::make_shared<std::vector<shptr<ClassMember>>>();
	p->push_back(std::make_shared<MethodDeclaration>(pr, pa, shptr<ast::Statement>()));

	// class #out
	lexer::Token oit {lexer::Token::Token_type::TOKEN_IDENT, lexer::Token::getTableReference("#out"), {0, 0}};
	auto oi = std::make_shared<Ident>(oit);
	auto o = std::make_shared<ClassDeclaration>(oi, p);
	classes->push_back(o);

	// #out out
	auto ft = std::make_shared<Type>(oi);
	lexer::Token fnt {lexer::Token::Token_type::TOKEN_IDENT, lexer::Token::getTableReference("out"), {0, 0}};
	auto fn = std::make_shared<Ident>(fnt);
	auto f = std::make_shared<std::vector<shptr<ClassMember>>>();
	f->push_back(std::make_shared<FieldDeclaration>(std::make_shared<TypeIdent>(ft, fn)));

	// class #System
	lexer::Token sit {lexer::Token::Token_type::TOKEN_IDENT, lexer::Token::getTableReference("#System"), {0, 0}};
	auto si = std::make_shared<Ident>(sit);
	auto s = std::make_shared<ClassDeclaration>(si, f);
	classes->push_back(s);
}
