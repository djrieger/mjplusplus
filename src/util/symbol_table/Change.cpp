#include "Change.hpp"

Change::Change(Symbol const &symbol, Definition const &previousDefinition, Scope const &previousScope): symbol(symbol), previousDefinition(previousDefinition), previousScope(previousScope) {}

Change::Change(Symbol const &symbol): symbol(symbol), previousDefinition(symbol.getCurrentDefinition()), previousScope(symbol.getCurrentScope()) {}

Symbol const &Change::getSymbol() const {
	return this->symbol;
}
