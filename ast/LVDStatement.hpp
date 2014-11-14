#ifndef LVD_STATEMENT_HPP
#define LVD_STATEMENT_HPP

#include "Expression.hpp"
#include "Statement.hpp"
#include "TypeIdent.hpp"

namespace ast
{
	class LVDStatement : public Statement
	{
		protected:
			std::unique_ptr<TypeIdent> type_ident;
			std::unique_ptr<Expression> init_expr;

		public:
			LVDStatement(std::unique_ptr<TypeIdent>& type_ident);
			LVDStatement(std::unique_ptr<TypeIdent>& type_ident, std::unique_ptr<Expression>& init_expr);
			virtual std::string toString(unsigned int indent) const;
	};
}

#endif
