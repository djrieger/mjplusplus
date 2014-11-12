#include "TypeIdent.hpp"


std::string ast::TypeIdent::toString()
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

	// first dimension with size, others just as []'s
	if (dimension >= 1)
	{
		if (array_size > 0)
			s += "[" + std::to_string(array_size) + "]";

		if (dimension >= 2)
		{
			for (int i = 0; i < dimension - 1; ++i)
				s += "[]";
		}
	}

	s += " " + identifier.toString();

	return s;
}

ast::TypeIdent::TypeIdent(Ident& identifier,
                          Primitive_type primitive_type,
                          std::string class_name,
                          int dimension,
                          int array_size):
	identifier(identifier), primitive_type(primitive_type), class_name(class_name), dimension(dimension), array_size(array_size)
{
}