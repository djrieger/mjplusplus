#include "PositionAwareNode.hpp"

ast::PositionAwareNode::PositionAwareNode(lexer::Token const& token): position(token.position)
{
}

ast::PositionAwareNode::PositionAwareNode(std::pair<unsigned int, unsigned int> position): position(position)
{
}

std::pair<unsigned int, unsigned int> ast::PositionAwareNode::getPosition() const
{
	return position;
}