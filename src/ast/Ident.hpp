#ifndef IDENT_HPP
#define IDENT_HPP

#include "../globals.hpp"
#include "Node.hpp"

namespace ast
{
	class Ident : public Node
	{
		private:
			std::string const& string_value;

		public:
			Ident(std::string const& string_value);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			virtual std::string getName() const;
			bool operator==(Ident const& other);
			bool operator!=(Ident const& other);
	};
}
#endif // IDENT_HPP
