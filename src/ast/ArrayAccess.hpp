#ifndef ARRAYACCESS_HPP
#define ARRAYACCESS_HPP

#include "Expression.hpp"
#include "PostfixOp.hpp"

namespace ast
{
	class ArrayAccess : public PostfixOp
	{
		private:
			uptr<Expression> access_offset;

		public:
			ArrayAccess(uptr<Expression> access_offset);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};
}

#endif
