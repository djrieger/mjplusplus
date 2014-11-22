#include "PositionAwareNode.hpp"

ast::PositionAwareNode::PositionAwareNode(lexer::Token const& token): position(token.position)
{
}

std::pair<unsigned int, unsigned int> ast::PositionAwareNode::getPosition() const
{
	return position;
}