#include "Scope.hpp"


Scope::Scope(shptr<Scope> parent, unsigned int oldSize): parent(parent), oldSize(oldSize) {}

unsigned int Scope::getOldSize() const
{
	return this->oldSize;
}

shptr<Scope> Scope::getParent()
{
	return parent;
}
