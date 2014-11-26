#ifndef TYPEIDENT_HPP
#define TYPEIDENT_HPP

#include "../globals.hpp"
#include "Node.hpp"
#include "Type.hpp"
#include "Ident.hpp"

namespace ast
{
	class TypeIdent : public Node
	{
		public:
			TypeIdent(shptr<Type> type, shptr<Ident> identifier);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual std::string getName() const;
			shptr<Type> const& getType() const;
			shptr<Ident> const& getIdent() const;
			virtual void accept(ASTVisitor& visitor) const;
		private:
			shptr<Type> type;
			shptr<Ident> identifier;
	};
}

#endif
