#ifndef INTEGER_LITERAL_HPP
#define INTEGER_LITERAL_HPP

#include "Node.hpp"

namespace ast
{
	class IntegerLiteral : public Node
	{
		private:
			std::string string_value;

		public:
			IntegerLiteral(std::string string_value);
			virtual std::string toString();
			// Konstruktor überladen mit token?
	}
}

#endif
