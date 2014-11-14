#include "ArrayType.hpp"

ast::ArrayType::ArrayType(std::unique_ptr<BasicType>& basic_type) : basic_type(std::move(basic_type))
{

}

std::string ast::ArrayType::toString() const
{
	return basic_type->toString() + "[]";
}