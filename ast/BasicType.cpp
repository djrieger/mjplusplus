#include "BasicType.hpp"

ast::BasicType::BasicType(Type::Primitive_type primitive_type) : primitive_type(primitive_type)
{

}

ast::BasicType::BasicType(std::string class_name) : primitive_type(Type::Primitive_type::NONE), class_name(class_name)
{

}

void ast::BasicType::toString(std::ostream& out, unsigned int, bool special) const
{
	switch (primitive_type)
	{
		case Type::Primitive_type::INT:
			out << "int";
			break;

		case Type::Primitive_type::BOOLEAN:
			out << "boolean";
			break;

		case Type::Primitive_type::VOID:
			out << "void";
			break;

		case Type::Primitive_type::NONE:
			out << class_name;
			break;
	}
}
