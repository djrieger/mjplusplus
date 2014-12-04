#include "../globals.hpp"
#include "LVDStatement.hpp"
#include "../firm_interface/FirmInterface.hpp"

ast::LVDStatement::LVDStatement(shptr<TypeIdent> type_ident)
	: type_ident(type_ident)
{

}
ast::LVDStatement::LVDStatement(shptr<TypeIdent> type_ident, shptr<Expression> init_expr)
	: type_ident(type_ident), init_expr(init_expr)
{
	;
}

shptr<ast::Type> const& ast::LVDStatement::getDeclType() const
{
	return type_ident->getType();
}

shptr<ast::Ident> ast::LVDStatement::getIdent() const
{
	return type_ident->getIdent();
}

shptr<ast::Expression> const& ast::LVDStatement::getInitialization() const
{
	return init_expr;
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
		sa.reportError("Symbol $ident{" + type_ident->getName() + "} already defined", type_ident->getIdent());
		return false;
	}

	auto type = type_ident->getType();

	if (!sa.isTypeDefined(type))
	{
		sa.reportError("Type $type{" + type->getName() + "} is not defined", type_ident->getIdent());
		return false;
	}

	auto d = std::make_shared<Definition>(s, std::static_pointer_cast<LVDStatement const>(shared_from_this()));
	symbolTable->insert(s, d);

	if (init_expr)
	{
		auto init_type = init_expr->get_type(sa, symbolTable);

		if (init_type)
		{
			if (init_type->getPrimitiveType() == ast::Type::Primitive_type::NULL_TYPE)
			{
				if (!type_ident->getType()->isRefType())
					sa.reportError("Cannot assign null to non-reference type $type{" + type_ident->getType()->getName() + "}", type_ident->getIdent());
			}
			else if (*type_ident->getType() != *init_type)
				sa.reportError("Mismatched Types: $type{" + type_ident->getType()->getName() + "} and $type{" + init_type->getName() + "}", type_ident->getIdent());
		}
		else
			sa.reportError("Invalid Type in assignment.", type_ident->getIdent());
	}

	return false;
}

void ast::LVDStatement::accept(ASTVisitor& visitor) const
{
	visitor.visit(std::static_pointer_cast<LVDStatement const>(shared_from_this()));
}

unsigned int ast::LVDStatement::countVariableDeclarations() const
{
	return 1;
}

int ast::LVDStatement::setVariablePositions(int pos) const
{
	(*FirmInterface::getInstance().getVarMap())[this->type_ident->getName()] = pos++;
	return pos;
}
