#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "../../globals.hpp"

#include "Scope.hpp"
#include "Definition.hpp"

class Symbol
{
	private:
		std::string const* name;
		shptr<Scope> currentScope;
		shptr<Definition> currentDefinition;
	public:
		shptr<Definition> getCurrentDefinition() const;
		shptr<Scope> getCurrentScope() const;

		void setCurrentDefinition(shptr<Definition> definition);
		void setCurrentScope(shptr<Scope> scope);

		Symbol(const std::string* name, shptr<Scope> currentScope, shptr<Definition> currentDefinition);
};

#endif