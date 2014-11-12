#ifndef METHOD_DECLARATION_HPP
#define METHOD_DECLARATION_HPP

#include "ClassMember.hpp"

namespace ast
{
	class MethodDeclaration : public ClassMember
	{
		protected:
			TypeIdent& return_type_and_name;
			std::vector<TypeIdent> parameters;
			Statement& block;

		public:
			MethodDeclaration(TypeIdent& return_type_and_name, std::vector<TypeIdent> parameters, Statement& block);
	}
}

#endif
