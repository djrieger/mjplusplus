#include <sstream>

#include "Type.hpp"
#include "Expression.hpp"

ast::Type::Type(Type::Primitive_type primitive_type): primitive_type(primitive_type), dimension(0) {}
ast::Type::Type(shptr<Ident> class_name): primitive_type(Type::Primitive_type::NONE), class_name(class_name), dimension(0) {}
ast::Type::Type(Type::Primitive_type primitive_type, int dimension): primitive_type(primitive_type), dimension(dimension) {}
ast::Type::Type(shptr<Ident> class_name, int dimension): primitive_type(Type::Primitive_type::NONE), class_name(class_name), dimension(dimension) {}

void ast::Type::toString(std::ostream& out, unsigned int indent, bool special) const
{
	shptr<Expression> dummy;
	toString(out, indent, dummy, special);
}

void ast::Type::toString(std::ostream& out, unsigned int indent, shptr<Expression> const& expression, bool) const
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
			class_name->toString(out, indent, false);
			break;

		case Type::Primitive_type::NULL_TYPE:
			//Only used for type analysis
			out << "null";
			break;
	}

	for (int i = 0; i < dimension; ++i)
	{
		out << "[";

		if (i == 0 && expression)
			expression->toString(out, indent, false);

		out << "]";
	}
}

void ast::Type::setDimension(int dimension)
{
	this->dimension = dimension;
}

int ast::Type::getDimension() const
{
	return dimension;
}

bool ast::Type::operator==(Type const& other)
{
	return !(*this != other);
}

bool ast::Type::operator!=(Type const& other)
{
	if (this->dimension != other.dimension ||
	        this->primitive_type != other.primitive_type)
		return true;

	return (this->primitive_type == ast::Type::Primitive_type::NONE) &&
	       (*this->class_name != *other.class_name);
}

bool ast::Type::isRefType(bool checkNullType)
{
	bool isRef = dimension > 0 ||
	             primitive_type == ast::Type::Primitive_type::NONE;

	if (!isRef && checkNullType)
		return primitive_type == ast::Type::Primitive_type::NULL_TYPE;

	return isRef;
}

bool ast::Type::isClassType()
{
	return dimension == 0 && primitive_type == ast::Type::Primitive_type::NONE;
}

bool ast::Type::isInteger()
{
	return dimension == 0 && primitive_type == ast::Type::Primitive_type::INT;
}

bool ast::Type::isBool()
{
	return dimension == 0 && primitive_type == ast::Type::Primitive_type::BOOLEAN;
}

shptr<ast::Type> ast::Type::de_array()
{
	if (dimension == 0)
		throw "Semantic error: cannot perform array access on non-array type";

	if (primitive_type == ast::Type::Primitive_type::NONE)
	{
		// TODO: Does this work?
		//auto ident = std::make_shared<ast::Ident>(class_name->getName());
		return std::make_shared<ast::Type>(class_name, dimension - 1);
	}
	else
		return std::make_shared<ast::Type>(primitive_type, dimension - 1);
}

ast::Type::Primitive_type ast::Type::getPrimitiveType() const
{
	return primitive_type;
}

std::string const& ast::Type::getClassName() const
{
	return class_name->getName();
}

shptr<ast::Ident> const& ast::Type::getClassNameIdent() const
{
	return class_name;
}

std::string ast::Type::getName() const
{
	std::stringstream descr;
	toString(descr, 0);
	return descr.str();
}
