#include "Definition.hpp"
#include "Symbol.hpp"

namespace semantic
{
	namespace symbol
	{
		shptr<Symbol> Definition::getSymbol() const
		{
			return symbol;
		}

		shptr<ast::Type> Definition::getType() const
		{
			return declaringNode ? declaringNode->getDeclType() : shptr<ast::Type>();
		}

		shptr<ast::VariableDeclaration const> Definition::getDeclaringNode() const
		{
			return declaringNode;
		}

		Definition::Definition(shptr<Symbol> symbol, shptr<ast::VariableDeclaration const> declaringNode): symbol(symbol), declaringNode(declaringNode) {}
	}
}
