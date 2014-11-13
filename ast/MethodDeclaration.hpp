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
			std::unique_ptr<TypeIdent> return_type_and_name;
			std::unique_ptr<std::vector<TypeIdent>> parameters;
			std::unique_ptr<Statement> block;

		public:
			MethodDeclaration(std::unique_ptr<TypeIdent> &return_type_and_name, std::unique_ptr<std::vector<TypeIdent>> &parameters, std::unique_ptr<Statement> &block);
			virtual std::string toString();
	};
}

#endif
