#include "MethodDeclaration.hpp"

namespace ast
{
	class MainMethodDeclaration : public MethodDeclaration
	{

		public:
			MainMethodDeclaration(Statement& block);
	}
}