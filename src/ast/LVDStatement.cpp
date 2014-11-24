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
	auto s = Symbol::makeSymbol(type_ident->getName());

	if (symbolTable->definedInCurrentScope(s) ||
	        (s->getCurrentDefinition() && s->getCurrentDefinition()->getType()->getName() != "$System"))
	{
		// formerly, there was s->getName(), however this segfaulted
		sa.reportError("Symbol " + type_ident->getName() + " already defined");
		return false;
	}

	auto type = type_ident->getType();

	if (!sa.isTypeDefined(type))
	{
		sa.reportError("Type " + type->getName() + " is not defined");
		return false;
	}

	auto d = std::make_shared<Definition>(s, type_ident->getType());
	symbolTable->insert(s, d);

	if (init_expr)
	{
		auto init_type = init_expr->get_type(sa, symbolTable);

		if (init_type)
		{
			if (init_type->getPrimitiveType() == ast::Type::Primitive_type::NULL_TYPE)
			{
				if (!type_ident->getType()->isRefType())
					sa.reportError("Cannot assign null to non-reference type " + type_ident->getType()->getName(), type_ident->getIdent());
			}
			else if (*type_ident->getType() != *init_type)
				sa.reportError("Mismatched Types: " + type_ident->getType()->getName() + " and " + init_type->getName(), type_ident->getIdent());
		}
		else
			sa.reportError("Invalid Type in assingment.", type_ident->getIdent());
	}

	return false;
}
