#ifndef IDENT_HPP
#define IDENT_HPP

#include "../globals.hpp"
#include "Node.hpp"
#include "PositionAwareNode.hpp"
#include "../lexer/token.hpp"

namespace ast
{
	class Ident : public Node, public PositionAwareNode
	{
		private:
			std::string const& string_value;

		public:
			/*
			 * Store this token's string value and its position
			 */
			Ident(lexer::Token const& token);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			std::string const& getName() const;
			bool operator==(Ident const& other);
			bool operator!=(Ident const& other);
	};
}
#endif // IDENT_HPP
