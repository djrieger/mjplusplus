#include "VisitorWithResultNode.hpp"

namespace firm
{
	namespace visitor
	{
		VisitorWithResultNode::VisitorWithResultNode() : resultNode(NULL), resultType(NULL), doStore(false), varNum(-1)
		{
			;
		}

		ir_node* VisitorWithResultNode::getResultNode() const
		{
			return resultNode;
		}

		ir_type* VisitorWithResultNode::getResultType() const
		{
			return resultType;
		}

		void VisitorWithResultNode::setDoStore(bool s)
		{
			doStore = s;
		}

		int VisitorWithResultNode::getVarNum() const
		{
			return varNum;
		}
	}
}
