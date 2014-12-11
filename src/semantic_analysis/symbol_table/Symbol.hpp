#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "../../globals.hpp"

#include "Scope.hpp"
#include "Definition.hpp"

namespace semantic
{
	namespace symbol
	{
		class Symbol
		{
			private:
				std::string const& name;
				shptr<Scope> currentScope;
				shptr<Definition> currentDefinition;

			public:
				shptr<Definition> getCurrentDefinition() const;
				shptr<Scope> getCurrentScope() const;
				std::string const& getName() const;

				void setCurrentDefinition(shptr<Definition> definition);
				void setCurrentScope(shptr<Scope> scope);

				static shptr<Symbol> makeSymbol(std::string const& name, shptr<Scope> scope = shptr<Scope>());

			private:
				Symbol(const std::string& name, shptr<Scope> currentScope, shptr<Definition> currentDefinition);
				Symbol(const std::string& name, shptr<Scope> currentScope);
		};
	}
}

#endif
