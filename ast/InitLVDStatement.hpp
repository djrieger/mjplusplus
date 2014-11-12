#ifndef INITLVDSTATEMENT_H
#define INITLVDSTATEMENT_H

#include "LVDStatement.hpp"
#include "TypeIdent.hpp"
#include "Expression.hpp"

namespace ast
{
	class InitLVDStatement : public LVDStatement
	{
		protected:
			Expression& init_expr;

		public:
			InitLVDStatement(TypeIdent& type_ident, Expression& init_expr);
			virtual std::string toString();
	};
}

#endif
