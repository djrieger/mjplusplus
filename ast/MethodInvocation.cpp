#include "MethodInvocation.hpp"

ast::MethodInvocation::MethodInvocation(std::unique_ptr<Ident>& method_name, std::unique_ptr<Arguments>& arguments)
	: method_name(std::move(method_name)), arguments(std::move(arguments))
{

}

std::string ast::MethodInvocation::toString(unsigned int indent) const
{
	return '.' + method_name->toString(indent) + "(" + arguments->toString(indent) + ")";
}
