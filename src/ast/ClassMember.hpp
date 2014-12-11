#ifndef CLASSMEMBER_HPP
#define CLASSMEMBER_HPP

#include "Node.hpp"
#include "../semantic_analysis/symbol_table/symbol_table.hpp"

namespace semantic
{
	class SemanticAnalysis;
}

namespace ast
{
	class ClassMember : public Node
	{
		protected:
			shptr<ClassDeclaration const> declaration;
		public:
			virtual std::string getNameForSort() const = 0;
			virtual std::string getName() const = 0;
			virtual void collectDefinition(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable, std::string const& class_name) const = 0;
			virtual void analyze(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable) const = 0;
			void setDeclaration(shptr<ClassDeclaration const> decl);
			shptr<ClassDeclaration const> getDeclaration() const;
			/**
			 * Returns declaration->getName() with $ replaced by _
			 */
			virtual std::string mangle() const;
	};
}

#endif
