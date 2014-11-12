#include "PostfixOp.hpp"

class ArrayAccess : public PostfixOp
{
	private:
		Expression& access_offset;

	public:
		ArrayAccess(Expression& access_offset;);
}