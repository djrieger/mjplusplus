#include "Definition.hpp"
#include "Symbol.hpp"

Symbol const& Definition::getSymbol() const
{
	return symbol;
}

ast::Type const& Definition::getType() const
{
	return type;
}

Definition::Definition(Symbol const& symbol, ast::Type const& type): symbol(symbol), type(type) {}
