#include "BasicType.hpp"

ast::BasicType::BasicType(Type::Primitive_type primitive_type) : primitive_type(primitive_type)
{

}

ast::BasicType::BasicType(std::string class_name) : primitive_type(Type::Primitive_type::NONE), class_name(class_name)
{

}

std::string ast::BasticType::toString()
{
	std::string s = "";

	switch (primitive_type)
	{
		case TypeIdent::Primitive_type::INT:
			s += "int";
			break;

		case TypeIdent::Primitive_type::BOOLEAN:
			s += "boolean";
			break;

		case TypeIdent::Primitive_type::VOID:
			s += "void";
			break;

		case TypeIdent::Primitive_type::NONE:
			s += class_name;
			break;
	}

	return s;
}
