#include "Node.hpp"

namespace ast
{
	class BoolLiteral : public Node
	{
		private:
			bool value;

		public:
			BoolLiteral(bool value);

			// Konstruktor überladen mit token?
	}
}