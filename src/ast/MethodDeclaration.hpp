#ifndef METHOD_DECLARATION_HPP
#define METHOD_DECLARATION_HPP

#include "../globals.hpp"
#include "ClassMember.hpp"
#include <vector>
#include "TypeIdent.hpp"
#include "Statement.hpp"

namespace ast
{
	class MethodDeclaration : public ClassMember
	{
		protected:
			uptr<TypeIdent> return_type_and_name;
			uptr<vec<uptr<TypeIdent>>> parameters;
			uptr<Statement> block;

		public:
			MethodDeclaration(uptr<TypeIdent> return_type_and_name, uptr<vec<uptr<TypeIdent>>> parameters, uptr<Statement> block);

			/*
			 * Prints this method including an optional "static" modifier (depending on isStatic)
			 */
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
			virtual std::string getName() const;
	};
}

#endif
