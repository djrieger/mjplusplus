#ifndef CHANGE_HPP
#define CHANGE_HPP

#include "Scope.hpp"
#include "Definition.hpp"
#include "Symbol.hpp"

class Change 
{
private:
	Symbol const &symbol;
	Definition const &previousDefinition;
	Scope const &previousScope;
public:
	Change(Symbol const &symbol, Definition const &previousDefinition, Scope const &previousScope);
	Change(Symbol const &symbol);

	Symbol const &getSymbol() const;
	Definition const &getPreviousDefinition() const;
	Scope const &getPreviousScope() const;
};

#endif