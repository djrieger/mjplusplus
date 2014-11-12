#include "PostfixOp.hpp"

namespace ast
{
	class ArrayAccess : public PostfixOp
	{
		private:
			Expression& access_offset;

		public:
			ArrayAccess(Expression& access_offset;);
	}
}