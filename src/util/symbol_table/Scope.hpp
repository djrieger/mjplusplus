#ifndef SCOPE_HPP
#define SCOPE_HPP

#include "../../globals.hpp"

class Scope
{
	private:
		shptr<Scope> parent;
		unsigned int oldSize;
	public:
		Scope(shptr<Scope> parent, unsigned int oldSize);

		unsigned int getOldSize() const;
		shptr<Scope> getParent();
};

#endif
