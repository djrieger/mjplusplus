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
			Ident(std::string const& string_value);
			virtual std::string toString(unsigned int indent) const;
	};
}
#endif // IDENT_HPP
