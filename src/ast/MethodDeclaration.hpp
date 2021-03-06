#ifndef METHOD_DECLARATION_HPP
#define METHOD_DECLARATION_HPP

#include "../globals.hpp"
#include "ClassMember.hpp"
#include <vector>
#include "TypeIdent.hpp"
#include "Statement.hpp"
#include "Block.hpp"
#include "LVDStatement.hpp"
#include "../firm_interface/FirmInterface.hpp"

namespace ast
{
	class MethodDeclaration : public ClassMember
	{
		protected:
			shptr<TypeIdent> return_type_and_name;
			shptr<vec<shptr<TypeIdent>>> parameters;
			shptr<stmt::Block> block;

		public:
			MethodDeclaration(shptr<TypeIdent> return_type_and_name, shptr<vec<shptr<TypeIdent>>> parameters, shptr<stmt::Block> block);

			/*
			 * Prints this method including an optional "static" modifier (depending on isStatic)
			 */
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;
			virtual std::string getName() const;
			virtual std::string getNameForSort() const;
			shptr<vec<shptr<TypeIdent>>> getParameters() const;
			shptr<Type> getReturnType() const;
			virtual void collectDefinition(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable, std::string const& class_name) const;
			virtual shptr<vec<shptr<Type>>> collectParameters(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable) const;
			virtual void analyze(semantic::SemanticAnalysis& sa, shptr<semantic::symbol::SymbolTable> symbolTable) const;
			shptr<stmt::Block> getBlock() const;
			virtual unsigned int countVariableDeclarations() const;
			virtual void createVariablePositions() const;
			/**
			 * Mangles this methods's name using ClassMember::mangle() and appends _Mmethodname
			 */
			virtual std::string mangle() const;

			virtual void accept(ASTVisitor& visitor) const;
	};
}

#endif
