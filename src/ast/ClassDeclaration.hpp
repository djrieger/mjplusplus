#ifndef CLASSDECLARATION_HPP
#define CLASSDECLARATION_HPP

#include <vector>

#include "Ident.hpp"
#include "ClassMember.hpp"
#include "Node.hpp"

namespace ast
{
	class ClassDeclaration : public Node
	{
		private:
			uptr<Ident> class_name;
			uptr<vec<uptr<ClassMember>>> members;

		public:
			ClassDeclaration(uptr<Ident>& class_name, uptr<vec<uptr<ClassMember>>>& members);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			std::string getName() const;
	};
}

#endif
