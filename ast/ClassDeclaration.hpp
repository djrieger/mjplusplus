#ifndef CLASSDECLARATION_H
#define CLASSDECLARATION_H

#include <vector>
#include <memory>
#include "Ident.hpp"
#include "ClassMember.hpp"
#include "Node.hpp"

namespace ast
{
	class ClassDeclaration : public Node
	{
		private:
			Ident class_name;
			std::unique_ptr<std::vector<ClassMember>> members;

		public:
			ClassDeclaration(Ident class_name, std::unique_ptr<std::vector<ClassMember>>&& members);
			virtual std::string toString();
	};
}

#endif
