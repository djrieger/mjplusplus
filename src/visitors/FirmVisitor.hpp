#ifndef FIRM_VISITOR_HPP
#define FIRM_VISITOR_HPP

#include <libfirm/firm.h>
#include "../firm_interface/FirmInterface.hpp"
#include "ASTVisitor.hpp"

class FirmVisitor : public ASTVisitor
{
	protected:
		ir_type* owner;
	public:
		FirmVisitor();
		virtual void setOwner(ir_type* owner);
		virtual ir_type* getOwner() const;
};

#endif //FIRM_VISITOR_HPP