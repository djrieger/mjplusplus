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
			std::unique_ptr<std::vector<ClassMember>> members;

		public:
			ClassDeclaration(std::unique_ptr<Ident>& class_name, std::unique_ptr<std::vector<ClassMember>>& members);
			virtual std::string toString();
	};
}

#endif
