#include "Symbol.hpp"

Symbol::Symbol(std::string const& name, shptr<Scope> currentScope, shptr<Definition> currentDefinition): name(name), currentScope(currentScope), currentDefinition(currentDefinition) {}

Symbol::Symbol(std::string const& name, shptr<Scope> currentScope): name(name), currentScope(currentScope), currentDefinition() {}


shptr<Definition> Symbol::getCurrentDefinition() const
{
	return currentDefinition;
}

shptr<Scope> Symbol::getCurrentScope() const
{
	return currentScope;
}

void Symbol::setCurrentDefinition(shptr<Definition> definition)
{
	this->currentDefinition = definition;
}

void Symbol::setCurrentScope(shptr<Scope> scope)
{
	this->currentScope = scope;
}