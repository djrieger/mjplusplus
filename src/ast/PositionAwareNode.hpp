#ifndef POSITION_AWARE_NODE_HPP
#define POSITION_AWARE_NODE_HPP

#include "../lexer/token.hpp"

namespace lexer
{
	class Token;
}

namespace ast
{
	/*
	 * AST node which has a (line, column) position from the original token it belongs to
	 */
	class PositionAwareNode
	{
		protected:
			source_position_t position;
		public:
			source_position_t getPosition() const;
			/*
			 * Stores the position of this token
			 */
			PositionAwareNode(lexer::Token const& token);
			PositionAwareNode(source_position_t position);
	};
}

#endif