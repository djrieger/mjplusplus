#ifndef FIELDDECLARATION_HPP
#define FIELDDECLARATION_HPP

#include "ClassMember.hpp"
#include "TypeIdent.hpp"

namespace ast
{
	class FieldDeclaration : public ClassMember
	{
		protected:
			std::unique_ptr<TypeIdent> type_and_name;

		public:
			FieldDeclaration(std::unique_ptr<TypeIdent>& type_and_name);

			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual std::string getName() const;
	};
}
#endif // FIELDDECLARATION_HPP
