#include "MethodDeclarion.hpp"

namespace ast
{
	class MainMethodDeclaration : public MethodDeclaration
	{

		public:
			MainMethodDeclaration(Statement& block);
	}
}