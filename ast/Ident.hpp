#ifndef IDENT_HPP
#define IDENT_HPP

#include "Node.hpp"

namespace ast
{
	class Ident : public Node
	{
		private:
			std::string string_value;

		public:
			Ident(std::string string_value);
			std::string toString();
	};
}
#endif // IDENT_HPP