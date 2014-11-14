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
			virtual std::string toString() const;

		private:
			std::unique_ptr<BasicType> basic_type;
			int dimension = 1;
	};
}

#endif