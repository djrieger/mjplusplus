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
			std::pair<unsigned int, unsigned int> position;
		public:
			void setPosition(lexer::Token const& token);
			std::pair<unsigned int, unsigned int> getPosition() const;
			/*
			 * Stores the position of this token
			 */
			PositionAwareNode(lexer::Token const& token);
	};
}

#endif