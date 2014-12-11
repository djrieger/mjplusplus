#include "symbol_table.hpp"
#include "../../ast/Type.hpp"
#include <iostream>

namespace semantic
{
	namespace symbol
	{
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
			changes.push(std::make_shared<Change>(symbol, symbol->getCurrentDefinition(), symbol->getCurrentScope()));
			symbol->setCurrentDefinition(def);
			symbol->setCurrentScope(this->currentScope);
		}

		void SymbolTable::enterScope()
		{
			this->currentScope = std::make_shared<Scope>(this->currentScope, this->changes.size());
		}

		void SymbolTable::leaveScope()
		{
			while (this->changes.size() > currentScope->getOldSize())
			{
				auto c = changes.top();
				c->getSymbol()->setCurrentDefinition(c->getPreviousDefinition());
				c->getSymbol()->setCurrentScope(c->getPreviousScope());

				changes.pop();
			}

			currentScope = currentScope->getParent();
		}

		bool SymbolTable::definedInCurrentScope(shptr<Symbol> symbol) const
		{
			return symbol->getCurrentDefinition() && symbol->getCurrentScope() == this->currentScope;
		}

		shptr<Scope> const& SymbolTable::getCurrentScope() const
		{
			return this->currentScope;
		}
	}
}
