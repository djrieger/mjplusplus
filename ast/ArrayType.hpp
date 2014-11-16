#ifndef ARRAYTYPE_HPP
#define ARRAYTYPE_HPP

#include "Type.hpp"
#include "BasicType.hpp"

namespace ast
{
	class ArrayType : public Type
	{

		public:
			ArrayType(std::unique_ptr<BasicType>& basic_type, int dimension);
			virtual void toString(std::ostream& out, unsigned int indent, bool special = false) const;

		private:
			std::unique_ptr<BasicType> basic_type;
			int dimension = 1;
	};
}

#endif