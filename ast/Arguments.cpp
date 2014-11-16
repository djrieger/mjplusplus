#include "Arguments.hpp"

ast::Arguments::Arguments(std::unique_ptr<std::vector<std::unique_ptr<Expression> > >& arguments) : arguments(std::move(arguments))
{

}

ast::Arguments::Arguments() : arguments(std::make_unique<std::vector<std::unique_ptr<Expression>>>())
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
