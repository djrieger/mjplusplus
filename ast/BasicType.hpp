#ifndef BASICTYPE_HPP
#define BASICTYPE_HPP

#include "Type.hpp"

namespace ast
{
	class BasicType : public Type
	{

		public:
			BasicType(Type::Primitive_type primitive_type);
			BasicType(std::string class_name);
			virtual void toString(std::ostream& out, unsigned int) const;

		private:
			Primitive_type primitive_type;
			std::string class_name; // maybe a pointer to the string table
	};
}

#endif
