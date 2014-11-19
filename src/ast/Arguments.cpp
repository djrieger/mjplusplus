#include "../globals.hpp"
#include "Arguments.hpp"

ast::Arguments::Arguments(uptr<vec<uptr<Expression> > > arguments) : arguments(std::move(arguments))
{

}

ast::Arguments::Arguments() : arguments(std::make_unique<vec<uptr<Expression>>>())
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
