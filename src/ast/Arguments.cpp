#include "../globals.hpp"
#include "Arguments.hpp"

ast::Arguments::Arguments(shptr<vec<shptr<Expression> > > arguments) : arguments(arguments)
{

}

ast::Arguments::Arguments() : arguments(std::make_shared<vec<shptr<Expression>>>())
{

}

void ast::Arguments::toString(std::ostream& out, unsigned int indent, bool) const
{
	out << "(";

	for (auto it = arguments->begin(); it != arguments->end(); it++)
	{
		(**it).toString(out, indent, true);

		if (it + 1 != arguments->end())
			out << ", ";
	}

	out << ")";
}

shptr<vec<shptr<ast::Expression>>> ast::Arguments::getArgumentTypes() const
{
	return arguments;
}
