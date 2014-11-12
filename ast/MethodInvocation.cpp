#include "MethodInvocation.hpp"

ast::MethodInvocation::MethodInvocation(Ident& method_name, std::vector<Expression> arguments)
	: method_name(method_name), arguments(arguments)
{

}

std::string ast::MethodInvocation::toString()
{
	std::string ret = method_name + "(";
	auto it = arguments.begin();

	while (1)
	{
		ret += it.toString();

		if (++it != arguments.end())
			ret += ", ";
		else
			break;
	}

	ret += ")";

	return ret;
}
