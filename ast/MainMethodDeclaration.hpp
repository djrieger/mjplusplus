#ifndef MAIN_METHOD_DECLARATION_HPP
#define MAIN_METHOD_DECLARATION_HPP

#include "MethodDeclaration.hpp"
#include "Statement.hpp"

namespace ast
{
	class MainMethodDeclaration : public MethodDeclaration
	{

		public:
			MainMethodDeclaration(Statement& block);
			virtual std::string toString();
	}
}

#endif
