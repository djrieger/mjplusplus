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
			uptr<TypeIdent> type_and_name;

		public:
			FieldDeclaration(uptr<TypeIdent> type_and_name);

			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual std::string getName() const;
	};
}
#endif // FIELDDECLARATION_HPP
