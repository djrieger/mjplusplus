#include "Type.hpp"

ast::Type::Type(Type::Primitive_type primitive_type): primitive_type(primitive_type), dimension(0) {}
ast::Type::Type(uptr<Ident> &class_name): primitive_type(Type::Primitive_type::NONE), class_name(std::move(class_name)), dimension(0) {}
ast::Type::Type(Type::Primitive_type primitive_type, int dimension): primitive_type(primitive_type), dimension(dimension) {}
ast::Type::Type(uptr<Ident> &class_name, int dimension): primitive_type(Type::Primitive_type::NONE), class_name(std::move(class_name)), dimension(dimension) {}

void ast::Type::toString(std::ostream& out, unsigned int indent, bool special) const
{
	uptr<Expression> dummy;
	toString(out, indent, dummy, special);
}

void ast::Type::toString(std::ostream& out, unsigned int indent, uptr<Expression> &expression, bool) const {
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
			class_name->toString(out, 0, false);
			break;
	}

	for (int i = 0; i < dimension; ++i) {
		out << "[";
		if (i == 1 && expression) {
			expression->toString(out, 0, false);
		}
		out << "]";	
	}
}

void ast::Type::setDimension(int dimension) {
	this->dimension = dimension;
}