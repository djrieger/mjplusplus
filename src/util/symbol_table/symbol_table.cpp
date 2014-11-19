#include "symbol_table.hpp"

Definition SymbolTable::lookup(Symbol &symbol) const {
	return symbol.getCurrentDefinition();
}

void SymbolTable::insert(Symbol &symbol, Definition const &def) {
	changes.push(symbol);
	symbol.setCurrentDefinition(def);
	symbol.setCurrentScope(this->currentScope);
}

void SymbolTable::enterScope() {
	Scope s(this->currentScope, this->changes.size());
	this->currentScope = s;
}

void SymbolTable::leaveScope() {
	while (this->changes.size() > currentScope.getOldSize())
	{
		Change c = changes.top();
		c.getSymbol().setCurrentDefinition(c.getPreviousDefinition());
		c.getSymbol().setCurrentScope(c.getPreviousScope());
		changes.pop();
	}
}

bool SymbolTable::definedInCurrentScope(Symbol symbol) const {
	return symbol.getCurrentScope() == this->currentScope;
}