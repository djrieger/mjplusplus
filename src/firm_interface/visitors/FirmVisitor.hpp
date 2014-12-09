#ifndef FIRM_VISITOR_HPP
#define FIRM_VISITOR_HPP

#include <libfirm/firm.h>
#include "../FirmInterface.hpp"
#include "../../ast/ASTVisitor.hpp"

namespace firm
{
	namespace visitor
	{

		class FirmVisitor : public ast::ASTVisitor
		{
			protected:
				ir_type* owner;
			public:
				FirmVisitor();
				virtual void setOwner(ir_type* owner);
				virtual ir_type* getOwner() const;
		};
	}
}

#endif //FIRM_VISITOR_HPP
