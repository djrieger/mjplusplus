#ifndef DEFINITION_HPP
#define DEFINITION_HPP

#include "../../ast/Node.hpp"
#include "../../ast/VariableDeclaration.hpp"

class Symbol;

namespace ast
{
	class Type;
}

class Definition
{
	private:
		shptr<Symbol> symbol;
		shptr<ast::VariableDeclaration const> declaringNode;
	public:
		shptr<Symbol> getSymbol() const;
		shptr<ast::Type> getType() const;
		shptr<ast::VariableDeclaration const> getDeclaringNode() const;

		Definition(shptr<Symbol> symbol, shptr<ast::VariableDeclaration const> declaringNode);
};

#endif
