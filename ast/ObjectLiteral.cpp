#include "ObjectLiteral.hpp"

ast::ObjectLiteral::ObjectLiteral(Object_Type object_type) : object_type(object_type) {

}

std::string ast::ObjectLiteral::toString() {
	return (object_type == Object_Type::THIS_OBJECT) ? "this" : "null";
}