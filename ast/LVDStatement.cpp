#include "LVDStatement.hpp"

ast::LVDStatement::LVDStatement(TypeIdent& type_ident)
	: type_ident(type_ident)
{

}


std::string ast::LVDStatement::toString()
{
	return type_ident.toString() + ";" + std::endl;
}
