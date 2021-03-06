#ifndef POSTFIXOP_HPP
#define POSTFIXOP_HPP
#include "Node.hpp"
#include "Type.hpp"
#include "../semantic_analysis/SemanticAnalysis.hpp"

namespace ast
{
	namespace po
	{
		class PostfixOp : public Node
		{

			public:
				virtual shptr<Type> get_type(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable, shptr<Type> callingType) = 0;
				virtual bool lValueHelp() const = 0;
		};
	}
}
#endif // POSTFIXOP_HPP
