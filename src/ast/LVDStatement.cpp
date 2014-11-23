#include "../globals.hpp"
#include "LVDStatement.hpp"

ast::LVDStatement::LVDStatement(shptr<TypeIdent> type_ident)
	: type_ident(type_ident)
{

}
ast::LVDStatement::LVDStatement(shptr<TypeIdent> type_ident, shptr<Expression> init_expr)
	: type_ident(type_ident), init_expr(init_expr)
{
	;
}

void ast::LVDStatement::toString(std::ostream& out, unsigned int indent, bool) const
{
	out << std::string(indent, '\t');
	type_ident->toString(out, indent);

	if (init_expr)
	{
		out << " = ";
		init_expr->toString(out, indent, true);
	}

	out << ";\n";
}

bool ast::LVDStatement::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
{
	//Test if symbol is in table
	auto s = Symbol::makeSymbol(type_ident->getName(), shptr<Scope>());

	if (symbolTable->definedInCurrentScope(s))
	{
		sa.printError("Symbol " + s->getName() + " already defined");
		return false;
	}

	auto type = type_ident->getType();

	if (!sa.isTypeDefined(type))
	{
		sa.printError("Type " + type->getName() + " is not defined");
		return false;
	}

	auto d = std::make_shared<Definition>(s, type_ident->getType());
	s->setCurrentDefinition(d);
	symbolTable->insert(s, d);

	auto init_type = init_expr->get_type(sa, symbolTable);

	if (init_type)
	{
		if (type_ident->getType() != init_type)
			sa.printError("Mismatched Types: " + type_ident->getType()->getName() + " and " + init_type->getName());
	}

	return false;
}
