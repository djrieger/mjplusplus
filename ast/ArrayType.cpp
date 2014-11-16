#include "ArrayType.hpp"

ast::ArrayType::ArrayType(std::unique_ptr<BasicType>& basic_type, int dimension) : basic_type(std::move(basic_type)), dimension(dimension)
{

}

void ast::ArrayType::toString(std::ostream& out, unsigned int indent, bool special) const
{
	basic_type->toString(out, indent);

	for (int i = 0; i < dimension; ++i)
		out << "[]";
}
