#ifndef ARRAYTYPE_HPP
#define ARRAYTYPE_HPP

#include "Type.hpp"
#include "BasicType.hpp"

namespace ast
{
	class ArrayType : public Type
	{

		public:
			ArrayType(std::unique_ptr<BasicType>& basic_type);
			virtual std::string toString();

		private:
			// dimension is implicitly defined to 1
			std::unique_ptr<BasicType> basic_type;
	};
}

#endif