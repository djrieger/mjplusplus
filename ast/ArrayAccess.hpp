#ifndef ARRAYACCESS_H
#define ARRAYACCESS_H

#include "Expression.hpp"
#include "PostfixOp.hpp"

namespace ast
{
	class ArrayAccess : public PostfixOp
	{
		private:
			uptr<Expression> access_offset;

		public:
			ArrayAccess(uptr<Expression>& access_offset);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};
}

#endif
