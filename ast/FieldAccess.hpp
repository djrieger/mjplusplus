#ifndef FIELDACCESS_H
#define FIELDACCESS_H

#include "PostfixOp.hpp"

namespace ast
{
	class FieldAccess : public PostfixOp
	{
		private:
			Ident& field_name;

		public:
			FieldAccess(Ident& field_name);
			std::string toString();
	};
}

#endif
