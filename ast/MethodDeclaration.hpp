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
			std::unique_ptr<std::vector<std::unique_ptr<TypeIdent>>> parameters;
			std::unique_ptr<Statement> block;
			/*
			 * Prints this method including an optional "static" modifier (depending on isStatic)
			 */
			virtual std::string toString(unsigned int indent, bool isStatic) const;

		public:
			MethodDeclaration(std::unique_ptr<TypeIdent>& return_type_and_name, std::unique_ptr<std::vector<std::unique_ptr<TypeIdent>>>& parameters, std::unique_ptr<Statement>& block);
			/*
			 * Prints this method without a "static" modifier
			 */

			virtual std::string toString(unsigned int indent) const;
			virtual std::string getName() const;
	};
}

#endif
