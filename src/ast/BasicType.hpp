#ifndef BASICTYPE_HPP
#define BASICTYPE_HPP

#include "../globals.hpp"
#include "Type.hpp"

namespace ast
{
	class BasicType : public Type
	{

		public:
			BasicType(Type::Primitive_type primitive_type, std::string const& = "");
			BasicType(std::string const& class_name);
			virtual void toString(std::ostream& out, unsigned int, bool = false) const;

		private:
			Primitive_type primitive_type;
			std::string const& class_name;
	};
}

#endif
