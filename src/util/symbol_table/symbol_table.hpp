#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <stack>
#include "../../globals.hpp"
#include "Change.hpp"
#include "Scope.hpp"
#include "Definition.hpp"
#include "Symbol.hpp"

class SymbolTable
{
	private:
		std::stack<shptr<Change>> changes;
		shptr<Scope> currentScope;
	public:
		/*
		 * return definition of symbol
		 */
		shptr<Definition> lookup(shptr<Symbol> symbol) const;
		void insert(shptr<Symbol> symbol, shptr<Definition> def);
		void enterScope();
		void leaveScope();
		bool definedInCurrentScope(shptr<Symbol> symbol) const;

		shptr<Scope> const& getCurrentScope() const;

		SymbolTable();
};

#endif