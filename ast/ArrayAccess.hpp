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
			virtual std::string toString() const;
	};
}

#endif
