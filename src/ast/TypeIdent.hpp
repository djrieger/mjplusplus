#ifndef TYPEIDENT_HPP
#define TYPEIDENT_HPP

#include "../globals.hpp"
#include "Node.hpp"
#include "Type.hpp"
#include "Ident.hpp"
#include "VariableDeclaration.hpp"

namespace ast
{
	class TypeIdent : public Node, public VariableDeclaration
	{
		public:
			TypeIdent(shptr<Type> type, shptr<Ident> identifier);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual std::string getName() const;
			virtual shptr<Type> const& getType() const;
			shptr<Ident> const& getIdent() const;
			virtual shptr<Type> const& getDeclType() const;
			virtual void accept(ASTVisitor& visitor) const;
		private:
			shptr<Type> type;
			shptr<Ident> identifier;
	};
}

#endif
