#ifndef MAIN_METHOD_DECLARATION_HPP
#define MAIN_METHOD_DECLARATION_HPP

#include "MethodDeclaration.hpp"
#include "Statement.hpp"

namespace ast
{
	class MainMethodDeclaration : public MethodDeclaration
	{
		public:
			MainMethodDeclaration(std::unique_ptr<TypeIdent>& return_type_and_name, std::unique_ptr<std::vector<std::unique_ptr<TypeIdent>>>& parameters, std::unique_ptr<Statement>& block);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};
}

#endif
