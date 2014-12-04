#ifndef FIELDDECLARATION_HPP
#define FIELDDECLARATION_HPP

#include "../globals.hpp"
#include "ClassMember.hpp"
#include "TypeIdent.hpp"
#include "VariableDeclaration.hpp"

namespace ast
{
	class FieldDeclaration : public ClassMember, public VariableDeclaration
	{
		protected:
			shptr<TypeIdent> type_and_name;

		public:
			FieldDeclaration(shptr<TypeIdent> type_and_name);

			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual void collectDefinition(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, std::string const& class_name) const;
			virtual void analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const;
			virtual std::string getName() const;
			virtual std::string getNameForSort() const;
			virtual void accept(ASTVisitor& visitor) const;
			virtual shptr<ast::Type> getType() const;
			virtual shptr<Type> const& getDeclType() const;
			/**
			 * Mangles this field's name using ClassMember::mangle() and appends _Ffieldname
			 */
			virtual std::string mangle() const;
	};
}
#endif // FIELDDECLARATION_HPP
