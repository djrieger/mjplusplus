#ifndef MAIN_METHOD_DECLARATION_HPP
#define MAIN_METHOD_DECLARATION_HPP

#include "../globals.hpp"
#include "MethodDeclaration.hpp"
#include "Statement.hpp"

namespace ast
{
	class MainMethodDeclaration : public MethodDeclaration
	{
		public:
			MainMethodDeclaration(uptr<TypeIdent> return_type_and_name, uptr<vec<uptr<TypeIdent>>> parameters, uptr<Statement> block);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};
}

#endif
