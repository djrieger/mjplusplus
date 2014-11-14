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
			virtual std::string toString(unsigned int indent) const;

		private:
			Primitive_type primitive_type;
			std::string class_name; // maybe a pointer to the string table
	};
}

#endif