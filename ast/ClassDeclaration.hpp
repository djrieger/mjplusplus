#ifndef CLASSDECLARATION_H
#define CLASSDECLARATION_H

#include <vector>

#include "Node.hpp"

namespace ast
{
	class ClassDeclaration : public Node
	{
		private:
			Ident& class_name;
			std::vector<ClassMember> members;

		public:
			ClassDeclaration(Ident& class_name, std::vector<ClassMember> members);
			std::string toString();
	};
}

#endif
