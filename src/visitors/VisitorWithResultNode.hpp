#ifndef VISITOR_WITH_RESULT_NODE_HPP
#define VISITOR_WITH_RESULT_NODE_HPP

#include "FirmVisitor.hpp"

class VisitorWithResultNode : public FirmVisitor
{
	protected:
		ir_node* resultNode;
		ir_type* resultType;

	public:
		virtual ir_node* getResultNode() const;
		virtual ir_type* getResultType() const;
};

#endif
