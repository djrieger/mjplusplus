#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <stack>
#include "Change.hpp"
#include "Scope.hpp"
#include "Definition.hpp"
#include "Symbol.hpp"

class SymbolTable
{
private:
	std::stack<Change> changes;
	Scope currentScope;
public:
	/* 
	 * return definition of symbol
	 */
	Definition lookup(Symbol &symbol) const;
	void insert(Symbol &symbol, Definition const &def);
	void enterScope();
	void leaveScope();
	bool definedInCurrentScope(Symbol &symbol) const;
};

#endif