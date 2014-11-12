#include "PostfixOp.hpp"

namespace ast
{
	class FieldAccess : public PostfixOp
	{
		private:
			Ident& field_name;

		public:
			FieldAccess(Ident& field_name);
	}
}