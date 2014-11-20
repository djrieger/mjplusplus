#include "Scope.hpp"


Scope::Scope(shptr<Scope> const parent, unsigned int oldSize): parent(parent), oldSize(oldSize) {}

unsigned int Scope::getOldSize() const
{
	return this->oldSize;
}