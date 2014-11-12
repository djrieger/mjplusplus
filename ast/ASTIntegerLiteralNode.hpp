#include "Node.hpp"

namespace ast
{
	class IntegerLiteral : public Node
	{
		private:
			std::string string_value;

		public:
			IntegerLiteral(std::string string_value);

			// Konstruktor überladen mit token?
	}
}