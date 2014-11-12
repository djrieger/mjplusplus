#include "Node.hpp"

namespace ast
{
	class InitLVDStatement : public LVDStatement
	{
		protected:
			Expression& init_expr;

		public:
			LVDStatement(TypeIdent& type_ident, Expression& init_expr);
	}
}