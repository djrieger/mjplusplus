#ifndef DEFINITION_HPP
#define DEFINITION_HPP

#include "../../ast/Type.hpp"
//#include "Symbol.hpp"

class Symbol;

class Definition
{
	private:
		shptr<Symbol>   symbol;
		shptr<ast::Type>  type;
		shptr<ast::Node> declaringNode;
	public:
		shptr<Symbol> getSymbol() const;
		shptr<ast::Type>  getType() const;

		Definition(shptr<Symbol> symbol, shptr<ast::Type> type);
		//Definition(Symbol const &symbol, shptr<ast::Node> declaringNode);
};

#endif