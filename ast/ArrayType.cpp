#include "ArrayType.hpp"

ast::ArrayType::ArrayType(std::unique_ptr<BasicType>& basic_type, int dimension) : basic_type(std::move(basic_type)), dimension(dimension)
{

}

std::string ast::ArrayType::toString() const
{
	std::string s = basic_type->toString();

	for (int i = 0; i < dimension; ++i)
		s += "[]";

	return s;
}