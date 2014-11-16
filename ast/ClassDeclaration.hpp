#ifndef CLASSDECLARATION_H
#define CLASSDECLARATION_H

#include <vector>

#include "Ident.hpp"
#include "ClassMember.hpp"
#include "Node.hpp"

namespace ast
{
	class ClassDeclaration : public Node
	{
		private:
			std::unique_ptr<Ident> class_name;
			std::unique_ptr<std::vector<std::unique_ptr<ClassMember>>> members;

		public:
			ClassDeclaration(std::unique_ptr<Ident>& class_name, std::unique_ptr<std::vector<std::unique_ptr<ClassMember>>>& members);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			std::string getName() const;
	};
}

#endif
