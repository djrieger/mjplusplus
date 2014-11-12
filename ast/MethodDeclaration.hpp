#ifndef METHOD_DECLARATION_HPP
#define METHOD_DECLARATION_HPP

#include "ClassMember.hpp"
#include <vector>
#include "TypeIdent.hpp"
#include "Statement.hpp"

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
            virtual std::string toString();
    }
}

#endif
