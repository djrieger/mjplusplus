#include "Node.hpp"

class ClassDeclaration : public Node
{
	private:
		Ident& class_name;
		std::vector<ClassMember> members;

	public:
		ClassDeclaration(Ident& class_name, std: vector<ClassMember> members);
}