#ifndef ARRAYACCESS_H
#define ARRAYACCESS_H

#include "Expression.hpp"
#include "PostfixOp.hpp"

namespace ast
{
	class ArrayAccess : public PostfixOp
	{
		private:
			Expression& access_offset;

		public:
			ArrayAccess(Expression& access_offset);
			std::string toString();
	};
}

#endif
