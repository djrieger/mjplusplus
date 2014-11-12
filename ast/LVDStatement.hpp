#ifndef LVD_STATEMENT_HPP
#define LVD_STATEMENT_HPP

#include "Node.hpp"

namespace ast
{
	class LVDStatement : public Statement
	{
		protected:
			TypeIdent& type_ident;

		public:
			LVDStatement(TypeIdent& type_ident);
	}
}

#endif