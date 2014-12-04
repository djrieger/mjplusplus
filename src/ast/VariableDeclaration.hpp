#ifndef VARIABLEDECLARATION_HPP
#define VARIABLEDECLARATION_HPP

#include "../globals.hpp"
#include "../visitors/ASTVisitor.hpp"

namespace ast
{
	class Type;

	class VariableDeclaration
	{
		protected:
			VariableDeclaration();
		public:
			virtual shptr<Type> const& getDeclType() const = 0;
			virtual void accept(ASTVisitor& astVisitor) const = 0;
	};
}
#endif // VARIABLEDECLARATION_HPP
