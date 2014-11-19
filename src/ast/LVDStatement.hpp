#ifndef LVD_STATEMENT_HPP
#define LVD_STATEMENT_HPP

#include "../globals.hpp"
#include "Expression.hpp"
#include "Statement.hpp"
#include "TypeIdent.hpp"

namespace ast
{
	class LVDStatement : public Statement
	{
		protected:
			uptr<TypeIdent> type_ident;
			uptr<Expression> init_expr;

		public:
			LVDStatement(uptr<TypeIdent> type_ident);
			LVDStatement(uptr<TypeIdent> type_ident, uptr<Expression> init_expr);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};
}

#endif
