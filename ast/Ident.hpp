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
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual std::string getName() const;
	};
}
#endif // IDENT_HPP
