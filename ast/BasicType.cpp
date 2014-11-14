#include "BasicType.hpp"

ast::BasicType::BasicType(Type::Primitive_type primitive_type) : primitive_type(primitive_type)
{

}

ast::BasicType::BasicType(std::string class_name) : primitive_type(Type::Primitive_type::NONE), class_name(class_name)
{

}

std::string ast::BasicType::toString(unsigned int) const
{
	std::string s = "";

	switch (primitive_type)
	{
		case Type::Primitive_type::INT:
			s += "int";
			break;

		case Type::Primitive_type::BOOLEAN:
			s += "boolean";
			break;

		case Type::Primitive_type::VOID:
			s += "void";
			break;

		case Type::Primitive_type::NONE:
			s += class_name;
			break;
	}

	return s;
}
