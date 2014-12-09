#include "VisitorWithResultNode.hpp"

namespace firm
{
	namespace visitor
	{
		ir_node* VisitorWithResultNode::getResultNode() const
		{
			return this->resultNode;
		}

		ir_type* VisitorWithResultNode::getResultType() const
		{
			return resultType;
		}
	}
}
