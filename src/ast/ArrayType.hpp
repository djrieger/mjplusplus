#ifndef ARRAYTYPE_HPP
#define ARRAYTYPE_HPP

#include "../globals.hpp"
#include "Type.hpp"
#include "BasicType.hpp"

namespace ast
{
	class ArrayType : public Type
	{

		public:
			ArrayType(uptr<BasicType>& basic_type, int dimension);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;

		private:
			uptr<BasicType> basic_type;
			int dimension = 1;
	};
}

#endif
