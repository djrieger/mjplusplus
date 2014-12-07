#include "VisitorWithResultNode.hpp"

ir_node* VisitorWithResultNode::getResultNode() const
{
	return this->resultNode;
}

ir_type* VisitorWithResultNode::getResultType() const
{
	return resultType;
}
