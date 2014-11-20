#include "Change.hpp"

Change::Change(shptr<Symbol> symbol, shptr<Definition> const previousDefinition, shptr<Scope> const previousScope): symbol(symbol), previousDefinition(previousDefinition), previousScope(previousScope) {}

Change::Change(shptr<Symbol> symbol): symbol(symbol), previousDefinition(symbol->getCurrentDefinition()), previousScope(symbol->getCurrentScope()) {}

shptr<Symbol> Change::getSymbol() const {
	return this->symbol;
}
shptr<Scope> const Change::getPreviousScope() const {

	return this->previousScope;
}

shptr<Definition> const Change::getPreviousDefinition() const {
	return this->previousDefinition;
}

