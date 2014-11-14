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

			virtual std::string toString(unsigned int indent) const;
			virtual std::string getName() const;
	};
}
#endif // FIELDDECLARATION_HPP
