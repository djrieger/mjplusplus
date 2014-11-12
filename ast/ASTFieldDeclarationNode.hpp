#include "ClassMember.hpp"

namespace ast
{
	class FieldDeclaration : public ClassMember
	{
		protected:
			TypeIdent& type_and_name;

		public:
			FieldDeclaration(TypeIdent& type_and_name);
	}
}