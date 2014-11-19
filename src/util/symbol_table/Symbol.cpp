#include "Symbol.hpp"

Symbol::Symbol(std::string const *name, Scope &currentScope, Definition &currentDefinition): name(name), currentScope(&currentScope), currentDefinition(&currentDefinition) {}

Definition const &Symbol::getCurrentDefinition() const
{
	return *currentDefinition;
}

Scope const &Symbol::getCurrentScope() const 
{
	return *currentScope;
}

void Symbol::setCurrentDefinition(Definition const &definition)
{
	this->currentDefinition = &definition;
}

void Symbol::setCurrentScope(Scope const &scope)
{
	this->currentScope = &scope;
}