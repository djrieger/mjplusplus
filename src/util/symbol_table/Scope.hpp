#ifndef SCOPE_HPP
#define SCOPE_HPP

#include "../../globals.hpp"

class Scope
{
	private:
		shptr<Scope> const parent;
		unsigned int oldSize;
	public:
		Scope(shptr<Scope> const parent, unsigned int oldSize);

		unsigned int getOldSize() const;
};

#endif