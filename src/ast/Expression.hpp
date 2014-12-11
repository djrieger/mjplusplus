#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "Node.hpp"
#include "Type.hpp"
#include "PositionAwareNode.hpp"
#include "../semantic_analysis/symbol_table/symbol_table.hpp"
#include "../semantic_analysis/SemanticAnalysis.hpp"

namespace ast
{
	class Expression : public Node, public PositionAwareNode
	{
		public:
			Expression(source_position_t position);
			/**
			* Idea:
			* When getting the type of an Expression, the Expression checks if it's own
			* and it's children Expressions type definitions are valid and then returns
			* it's type. If something is invalid, an error is printed in the Expression
			* that is invalid and an empty pointer is returned.
			*/
			virtual shptr<Type> get_type(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable) const = 0;
			virtual bool isLValue() const = 0;
			virtual bool standalone() const = 0;
			/**
			 * Special while check for definite return analysis:
			 * Return a pair<bool,bool>. The first bool signifies whether it is a constant
			 * bool expression, and in this case, the second bool is the result.
			 */
			virtual std::pair<bool, bool> constBool() const = 0;

			virtual void accept(ASTVisitor& visitor) const;
	};
}

#endif
