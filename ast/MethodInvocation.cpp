#include "MethodInvocation.hpp"

ast::MethodInvocation::MethodInvocation(std::unique_ptr<Ident>& method_name, std::unique_ptr<Arguments>& arguments)
	: method_name(std::move(method_name)), arguments(std::move(arguments))
{

}

void ast::MethodInvocation::toString(std::ostream& out, unsigned int indent, bool special) const
{
	out << '.';
	method_name->toString(out, indent);
	arguments->toString(out, indent);
}
