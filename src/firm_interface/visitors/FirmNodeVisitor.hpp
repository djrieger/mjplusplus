#ifndef FIRM_NODE_VISITOR_HPP
#define FIRM_NODE_VISITOR_HPP

#include <libfirm/firm.h>

class FirmNodeVisitor
{
	protected:
		virtual void visitConstNode(ir_node* node);
		virtual void visitAddNode(ir_node* node);
	public:
		virtual void visit(ir_node* node);
};

#endif