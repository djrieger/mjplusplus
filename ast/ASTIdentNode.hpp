#include "Node.hpp"

namespace ast
{
	class Ident : public Node
	{
		private:
			std::string string_value;

		public:
			Ident(std::string string_value);

			// Konstruktor überladen mit token?
	}
}