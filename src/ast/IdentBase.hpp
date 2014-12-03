#ifndef IDENTBASE_HPP
#define IDENTBASE_HPP

#include "../globals.hpp"
#include "VariableDeclaration.hpp"

#include "FieldDeclaration.hpp"

namespace ast
{
	class IdentBase
	{
		protected:
			IdentBase();
			mutable shptr<VariableDeclaration const> declaration;
	};
}

#endif // IDENTBASE_HPP
