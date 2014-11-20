#ifndef CHANGE_HPP
#define CHANGE_HPP

#include "../../globals.hpp"
#include "Scope.hpp"
#include "Definition.hpp"
#include "Symbol.hpp"

class Change
{
	private:
		shptr<Symbol> symbol;
		shptr<Definition> const previousDefinition;
		shptr<Scope> const previousScope;
	public:
		Change(shptr<Symbol> symbol, shptr<Definition> const previousDefinition, shptr<Scope> const previousScope);
		Change(shptr<Symbol> symbol);

		shptr<Symbol> getSymbol() const;
		shptr<Definition> const getPreviousDefinition() const;
		shptr<Scope> const getPreviousScope() const;
};

#endif