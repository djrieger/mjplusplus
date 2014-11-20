#ifndef FIELDDECLARATION_HPP
#define FIELDDECLARATION_HPP

#include "../globals.hpp"
#include "ClassMember.hpp"
#include "TypeIdent.hpp"

namespace ast
{
	class FieldDeclaration : public ClassMember
	{
		protected:
			shptr<TypeIdent> type_and_name;

		public:
			FieldDeclaration(shptr<TypeIdent> type_and_name);

			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual void collectDefinition(shptr<SemanticAnalysis> sa, shptr<SymbolTable> symbolTable) const;
			virtual std::string getName() const;
	};
}
#endif // FIELDDECLARATION_HPP
