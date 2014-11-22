#ifndef POSITION_AWARE_NODE_HPP
#define POSITION_AWARE_NODE_HPP

#include "../lexer/token.hpp"

namespace lexer
{
	class Token;
}

namespace ast
{
	class PositionAwareNode
	{
		protected:
			std::pair<unsigned int, unsigned int> position;
		public:
			void setPosition(lexer::Token const& token);
			std::pair<unsigned int, unsigned int> getPosition() const;
			PositionAwareNode(lexer::Token const& token);
	};
}

#endif