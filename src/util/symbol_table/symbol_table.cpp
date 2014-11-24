#include "symbol_table.hpp"
#include "../../ast/Type.hpp"
#include <iostream>

SymbolTable::SymbolTable()
{
	enterScope();
}

shptr<Definition> SymbolTable::lookup(shptr<Symbol> symbol) const
{
	return symbol->getCurrentDefinition();
}

#define dp(def) ((def)?(def)->getType()->getName():(""))

void SymbolTable::insert(shptr<Symbol> symbol, shptr<Definition> def)
{
	//std::cout << "insert: " << symbol->getName() << " - " << dp(symbol->getCurrentDefinition()) << " - " << dp(def);
	changes.push(std::make_shared<Change>(symbol, symbol->getCurrentDefinition(), symbol->getCurrentScope()));
	symbol->setCurrentDefinition(def);
	//std::cout << " - " << dp(symbol->getCurrentDefinition()) << std::endl;
	symbol->setCurrentScope(this->currentScope);
}

void SymbolTable::enterScope()
{
	this->currentScope = std::make_shared<Scope>(this->currentScope, this->changes.size());
}

void SymbolTable::leaveScope()
{
	while (this->changes.size() > currentScope->getOldSize())
		//while (!this->changes.empty())
	{
		auto c = changes.top();
		//std::cout << "leave: " << c->getSymbol()->getName() << " - " << dp(c->getSymbol()->getCurrentDefinition()) << " - " << dp(c->getPreviousDefinition());
		c->getSymbol()->setCurrentDefinition(c->getPreviousDefinition());
		c->getSymbol()->setCurrentScope(c->getPreviousScope());

		//std::cout << " - " << dp(c->getSymbol()->getCurrentDefinition()) << std::endl;
		changes.pop();

	}

	currentScope = currentScope->getParent();
}

bool SymbolTable::definedInCurrentScope(shptr<Symbol> symbol) const
{
	return symbol->getCurrentScope() == this->currentScope;
}

shptr<Scope> const& SymbolTable::getCurrentScope() const
{
	return this->currentScope;
}
