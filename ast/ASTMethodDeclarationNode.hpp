#include "ClassMember.hpp"

class MethodDeclaration : public ClassMember
{
	protected:
		TypeIdent& return_type_and_name;
		std::vector<TypeIdent> parameters;
		Statement& block;

	public:
		MethodDeclaration(TypeIdent& return_type_and_name, std::vector<TypeIdent> parameters, Statement& block);
}