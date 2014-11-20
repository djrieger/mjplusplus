#ifndef DEFINITION_HPP
#define DEFINITION_HPP

#include "../../ast/Type.hpp"
//#include "Symbol.hpp"

class Symbol;

class Definition
{
	private:
		Symbol const& symbol;
		ast::Type const& type;
		shptr<ast::Node> declaringNode;
	public:
		Symbol const& getSymbol() const;
		ast::Type const& getType() const;

		//Definition(Symbol const &symbol, ast::Type const &type, shptr<ast::Node> declaringNode);
		//Definition(Symbol const &symbol, shptr<ast::Node> declaringNode);
};

#endif