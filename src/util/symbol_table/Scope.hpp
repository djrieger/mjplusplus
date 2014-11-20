#ifndef SCOPE_HPP
#define SCOPE_HPP

#include "../../globals.hpp"

class Scope
{
private:
	shptr<Scope> const parent;
	int oldSize;
public:
	Scope(shptr<Scope> const parent, int oldSize);

	int getOldSize() const;
};

#endif