#include "PositionAwareNode.hpp"

ast::PositionAwareNode::PositionAwareNode(lexer::Token const& token): position(token.position)
{
}

ast::PositionAwareNode::PositionAwareNode(source_position_t const& position): position(position)
{
}

source_position_t ast::PositionAwareNode::getPosition() const
{
	return position;
}