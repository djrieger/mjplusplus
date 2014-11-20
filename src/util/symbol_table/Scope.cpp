#include "Scope.hpp"


Scope::Scope(shptr<Scope> const parent, int oldSize): parent(parent), oldSize(oldSize) {}

int Scope::getOldSize() const
{
	return this->oldSize;
}