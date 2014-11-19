#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "Scope.hpp"
#include "Definition.hpp"

//class Definition;

class Symbol
{
	private:
		std::string const *name;
		Scope const *currentScope;
		Definition const *currentDefinition;
	public:
		Definition const &getCurrentDefinition() const;
		Scope const &getCurrentScope() const;

		void setCurrentDefinition(Definition const &definition);
		void setCurrentScope(Scope const &scope);

		Symbol(const std::string const *name, Scope &currentScope, Definition &currentDefinition);
};

#endif