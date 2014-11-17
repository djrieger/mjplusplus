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
			TypeIdent(uptr<Type>& type, uptr<Ident>& identifier);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual std::string getName() const;
		private:
			uptr<Type> type;
			uptr<Ident> identifier;
	};
}

#endif
