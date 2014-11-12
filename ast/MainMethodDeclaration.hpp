#ifndef MAIN_METHOD_DECLARATION_HPP
#define MAIN_METHOD_DECLARATION_HPP

#include "MethodDeclaration.hpp"

namespace ast
{
	class MainMethodDeclaration : public MethodDeclaration
	{

		public:
			MainMethodDeclaration(Statement& block);
	}
}

#endif
