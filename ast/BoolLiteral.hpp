#ifndef BOOLLITERAL_H
#define BOOLLITERAL_H

#include "Node.hpp"

namespace ast
{
	class BoolLiteral : public Node
	{
		private:
			bool value;

		public:
			BoolLiteral(bool value);
			virtual std::string toString();
	};
}

#endif
