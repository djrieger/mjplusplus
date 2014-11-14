#include "Arguments.hpp"

ast::Arguments::Arguments(std::unique_ptr<std::vector<std::unique_ptr<Expression> > >& arguments) : arguments(std::move(arguments))
{

}

ast::Arguments::Arguments() : arguments(std::make_unique<std::vector<std::unique_ptr<Expression>>>())
{

}

std::string ast::Arguments::toString(unsigned int indent) const
{
	std::string ret = "(";

	for (auto it = arguments->begin(); it != arguments->end(); it++)
	{
		ret += (**it).toString(indent);

		if (it + 1 != arguments->end())
			ret += ", ";
	}

	ret += ")";

	return ret;
}
