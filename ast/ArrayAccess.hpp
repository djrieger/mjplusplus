#ifndef ARRAYACCESS_H
#define ARRAYACCESS_H

#include "Expression.hpp"
#include "PostfixOp.hpp"

namespace ast
{
	class ArrayAccess : public PostfixOp
	{
		private:
			std::unique_ptr<Expression> access_offset;

		public:
			ArrayAccess(std::unique_ptr<Expression>& access_offset);
			virtual void toString(std::ostream& out, unsigned int indent) const;
	};
}

#endif
