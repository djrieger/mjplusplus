#include "Definition.hpp"
#include "Symbol.hpp"

shptr<Symbol>  Definition::getSymbol() const
{
	return symbol;
}

shptr<ast::Type>  Definition::getType() const
{
	return type;
}

Definition::Definition(shptr<Symbol> symbol, shptr<ast::Type> type): symbol(symbol), type(type) {}
