#include "Scope.hpp"


Scope::Scope(Scope const &parent, int oldSize): parent(parent), oldSize(oldSize) {}

int Scope::getOldSize() const
{
	return this->oldSize;
}