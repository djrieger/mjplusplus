#include "MethodInvocation.hpp"

ast::MethodInvocation::MethodInvocation(std::unique_ptr<Ident>& method_name, std::unique_ptr<std::vector<std::unique_ptr<Expression>>>& arguments)
	: method_name(std::move(method_name)), arguments(std::move(arguments))
{

}

std::string ast::MethodInvocation::toString()
{
	std::string ret = method_name->toString() + "(";
	auto it = arguments->begin();

	while (1)
	{
		ret += (**it).toString();

		if (++it != arguments->end())
			ret += ", ";
		else
			break;
	}

	ret += ")";

	return ret;
}
