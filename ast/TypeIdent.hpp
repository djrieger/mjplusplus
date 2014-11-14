#ifndef TYPEIDENT_HPP
#define TYPEIDENT_HPP

#include "Node.hpp"
#include "Type.hpp"
#include "Ident.hpp"

namespace ast
{
	class TypeIdent : public Node
	{

		public:
			TypeIdent(std::unique_ptr<Type>& type, std::unique_ptr<Ident>& identifier);
			virtual std::string toString(unsigned int indent) const;
			virtual std::string getName() const;
		private:
			std::unique_ptr<Type> type;
			std::unique_ptr<Ident> identifier;
	};
}

#endif