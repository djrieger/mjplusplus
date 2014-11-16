#ifndef FIELDACCESS_H
#define FIELDACCESS_H

#include "PostfixOp.hpp"
#include "Ident.hpp"

namespace ast
{
	class FieldAccess : public PostfixOp
	{
		private:
			std::unique_ptr<Ident> field_name;

		public:
			FieldAccess(std::unique_ptr<Ident>& field_name);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};
}

#endif
